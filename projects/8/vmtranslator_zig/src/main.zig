const std = @import("std");
const vmtranslator: type = @import("vmtranslator");

var gpa = std.heap.GeneralPurposeAllocator(.{}){};

pub fn main() !void {
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (2 != args.len) unreachable;
    var t = try vmtranslator.VmTranslator.init(allocator);
    defer t.deinit();

    try t.start(args[1]);
}
