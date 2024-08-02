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
            var iter = std.mem.tokenizeAny(u8, input, "\\/");
            var temp_file: []const u8 = "";
            while (iter.next()) |n| temp_file = n;
            const output = try std.fmt.allocPrint(self.allocator, "{s}{s}.asm", .{ input, temp_file });
            defer self.allocator.free(output);
            try self.coder.setFileName(output);
            try self.startDir(input);
        } else {
            if (!std.mem.eql(u8, ".vm", input[input.len - 3 .. input.len])) {
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
        }

        try std.io.getStdOut().writeAll("Done\n");
    }

    fn preview(self: *Self, input: []const u8) !void {
        if ('/' == input[0] or ':' == input[1]) {
            try self.parser.openFileAbsolute(input);
        } else {
            try self.parser.openFile(input);
        }
        defer self.parser.closeFile();

        while (self.parser.hasMoreCommands()) {
            if (parse.CommandType.C_FUNCTION != self.parser.commandType().?) continue;
            try self.coder.recoredFunction(self.parser.command.?);
        }
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
            try self.preview(file_name);
        }

        iterator.reset();
        while (try iterator.next()) |file| {
            if (!std.mem.eql(u8, ".vm", file.name[file.name.len - 3 .. file.name.len])) continue;
            if (std.fs.Dir.Entry.Kind.file != file.kind) continue;
            const file_name = try std.fmt.allocPrint(self.allocator, "{s}{s}", .{ input, file.name });
            defer self.allocator.free(file_name);
            try self.startFile(file_name);
        }
    }

    fn startFile(self: *Self, input: []const u8) !void {
        if ('/' == input[0] or ':' == input[1]) {
            try self.parser.openFileAbsolute(input);
        } else {
            try self.parser.openFile(input);
        }
        defer self.parser.closeFile();
        while (self.parser.hasMoreCommands()) {
            switch (self.parser.commandType().?) {
                parse.CommandType.C_ARITHMETIC => try self.coder.wrtieArithmetic(self.parser.command.?),
                parse.CommandType.C_PUSH, parse.CommandType.C_POP => try self.coder.writePushPop(self.parser.command.?, self.parser.file),
                parse.CommandType.C_FUNCTION => try self.coder.writeFunction(self.parser.command.?),
                parse.CommandType.C_GOTO => try self.coder.writeGoto(self.parser.command.?),
                parse.CommandType.C_IF => try self.coder.writeIf(self.parser.command.?),
                parse.CommandType.C_LABEL => try self.coder.writeLabel(self.parser.command.?),
                parse.CommandType.C_CALL => try self.coder.writeCall(self.parser.command.?),
                parse.CommandType.C_RETURN => try self.coder.writeReturn(),
            }
        }
    }

    pub fn deinit(self: *Self) void {
        self.parser.deinit();
        self.coder.deinit();
    }
};
