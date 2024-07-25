const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "vmtranslator",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    b.installArtifact(exe);

    const cmd = b.addRunArtifact(exe);
    const run_step = b.step("run", "");
    run_step.dependOn(&cmd.step);

    if (b.args) |args| {
        cmd.addArgs(args);
    }
}
