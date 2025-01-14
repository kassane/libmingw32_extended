const std = @import("std");

pub fn build(b: *std.Build) void {
    if (comptime !checkVersion())
        @compileError("Please! Update zig toolchain >= 0.11!");

    const tests = b.option(bool, "Tests", "Build Tests [default: false]") orelse false;

    const target = b.standardTargetOptions(.{
        .default_target = .{
            .cpu_arch = .x86_64,
            .os_tag = .windows,
            .abi = .gnu,
        },
    });
    const optimize = b.standardOptimizeOption(.{});

    const libposixGW = b.addStaticLibrary(.{
        .name = "mingw32_extended",
        .target = target,
        .optimize = optimize,
    });

    switch (optimize) {
        .Debug, .ReleaseSafe => libposixGW.bundle_compiler_rt = true,
        else => libposixGW.strip = true,
    }
    libposixGW.want_lto = false;
    libposixGW.addIncludePath(.{ .path = "include" });
    libposixGW.defineCMacro("__USE_MINGW_ANSI_STDIO", "1");
    libposixGW.addCSourceFiles(&.{
        "source/writev.c",
        "source/dlsym.c",
        "source/posix_madvise.c",
        "source/msync.c",
        "source/wait.c",
        "source/munlock.c",
        "source/munmap.c",
        "source/dladdr.c",
        "source/dlclose.c",
        "source/mlock.c",
        "source/mmap.c",
        "source/process_vm_readv.c",
        "source/pipe.c",
        "source/readv.c",
        "source/dlopen.c",
        "source/process_vm_writev.c",
        "source/dlerror.c",
        "source/shm_open.c",
        "source/madvise.c",
        "source/mprotect.c",
        "source/getpagesize.c",
        "source/shm_unlink.c",
    }, &.{
        "-Wall",
        "-Wextra",
    });
    const winpthreads_dep = b.dependency("winpthreads", .{
        .optimize = optimize,
        .target = target,
    });
    const winpthreads = winpthreads_dep.artifact("winpthreads");
    libposixGW.linkLibrary(winpthreads);
    libposixGW.installLibraryHeaders(winpthreads);
    libposixGW.linkLibC();
    libposixGW.installHeadersDirectory("include", "");
    b.installArtifact(libposixGW);

    if (tests) {
        buildExe(b, libposixGW, .{
            .name = "test_process_vm_readv",
            .file = "tests/test_process_vm_readv.c",
        });
        buildExe(b, libposixGW, .{
            .name = "test_mmap",
            .file = "tests/test_mmap.c",
        });
    }
}

fn buildExe(b: *std.Build, lib: *std.Build.Step.Compile, binfo: BuildInfo) void {
    const exe = b.addExecutable(.{
        .name = binfo.name,
        .target = lib.target,
        .optimize = lib.optimize,
    });
    if (lib.optimize != .Debug)
        exe.strip = true;
    exe.want_lto = false;
    for (lib.include_dirs.items) |include| {
        exe.include_dirs.append(include) catch {};
    }
    exe.linkLibrary(lib);
    exe.addCSourceFile(.{
        .file = .{ .path = binfo.file },
        .flags = &.{
            "-Wall",
            "-Wextra",
            "-Wpedantic",
        },
    });
    exe.linkLibC();
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step(binfo.name, b.fmt("Run the {s} app.", .{binfo.name}));
    run_step.dependOn(&run_cmd.step);
}

const BuildInfo = struct {
    name: []const u8,
    file: []const u8,
};

fn checkVersion() bool {
    const builtin = @import("builtin");
    if (!@hasDecl(builtin, "zig_version")) {
        return false;
    }

    const needed_version = std.SemanticVersion.parse("0.11.0") catch unreachable;
    const version = builtin.zig_version;
    const order = version.order(needed_version);
    return order != .lt;
}
