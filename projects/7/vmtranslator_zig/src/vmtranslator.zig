const std = @import("std");
const parse = @import("parse.zig");
const code = @import("code.zig");

pub const VmTranslator = struct {
    parser: parse.Parse,
    coder: code.Code,
    allocator: std.mem.Allocator,

    const Self = @This();

    pub fn init(allocator: std.mem.Allocator) !Self {
        return Self{
            .allocator = allocator,
            .parser = try parse.Parse.init(allocator),
            .coder = try code.Code.init(allocator),
        };
    }

    pub fn start(self: *Self, input: []const u8) !void {
        if ('/' == input[input.len - 1] or '\\' == input[input.len - 1]) {
            const output = try std.fmt.allocPrint(self.allocator, "{s}{s}", .{ input, "output.asm" });
            defer self.allocator.free(output);
            try self.coder.setFileName(output);
            try self.startDir(input);
        } else {
            if (!std.mem.eql(u8, ".vm", input[input.len - 3 .. input.len])) {
                // std.debug.print("{s} {c} {c} {c}\n", .{ input, input[input.len - 3], input[input.len - 2], input[input.len - 1] });
                unreachable;
            }
            const output: []u8 = try self.allocator.alloc(u8, input.len + 1);
            defer self.allocator.free(output);
            _ = try std.fmt.bufPrint(output, "{s}", .{input});
            output[input.len] = 'm';
            output[input.len - 1] = 's';
            output[input.len - 2] = 'a';
            try self.coder.setFileName(output);
            try self.startFile(input);
            // std.debug.print("aha\n", .{});
        }

        try std.io.getStdOut().writeAll("Done\n");
    }

    fn startDir(self: *Self, input: []const u8) !void {
        var dir: std.fs.Dir = blk: {
            if ('/' == input[0] or ':' == input[1]) {
                break :blk try std.fs.openDirAbsolute(input, .{ .iterate = true });
            } else {
                break :blk try std.fs.cwd().openDir(input, .{ .iterate = true });
            }
        };
        var iterator: std.fs.Dir.Iterator = dir.iterate();

        while (try iterator.next()) |file| {
            if (!std.mem.eql(u8, ".vm", file.name[file.name.len - 3 .. file.name.len])) continue;
            if (std.fs.Dir.Entry.Kind.file != file.kind) continue;
            const file_name = try std.fmt.allocPrint(self.allocator, "{s}{s}", .{ input, file.name });
            defer self.allocator.free(file_name);
            // std.mem.copyForwards(u8, file_name[0..input.len], input);
            // std.mem.copyForwards(u8, file_name[input.len..], file.name);
            try self.startFile(file_name);
        }
    }

    fn startFile(self: *Self, input: []const u8) !void {
        var _input: std.fs.File = blk: {
            // for (input) |c| std.debug.print("{c}", .{c});
            // std.debug.print("\n", .{});
            if ('/' == input[0] or ':' == input[1]) {
                break :blk try std.fs.openFileAbsolute(input, .{});
            } else {
                break :blk try std.fs.cwd().openFile(input, .{});
            }
        };
        defer _input.close();
        try self.parser.openFile(&_input);
        defer self.parser.closeFile();
        // std.debug.print("goto\n", .{});
        while (self.parser.hasMoreCommands()) {
            // std.debug.print("once\n", .{});
            switch (self.parser.commandType().?) {
                parse.CommandType.C_ARITHMETIC => try self.coder.wrtieArithmetic(self.parser.command.?),
                parse.CommandType.C_PUSH, parse.CommandType.C_POP => try self.coder.writePushPop(self.parser.command.?),
                else => {
                    // std.debug.print("here\n", .{});
                },
            }
        }
    }

    pub fn deinit(self: *Self) void {
        self.parser.deinit();
        self.coder.deinit();
    }
};
