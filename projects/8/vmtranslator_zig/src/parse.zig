const std = @import("std");

pub const CommandType = enum {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
};

pub const Parse = struct {
    allocator: std.mem.Allocator,
    command_iterator: ?std.mem.TokenIterator(u8, .any),
    content: ?[]u8,
    command: ?[]const u8,
    command_type: ?CommandType,
    argument1: ?[]const u8,
    argument2: u32,
    argument1_table: std.StringHashMap(CommandType),
    content_table: std.StringHashMap([]u8),
    file: []const u8,

    const Self = @This();

    fn countArguments(self: *Self) bool {
        var arguments_iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, self.command.?, " \t");

        if (arguments_iterator.next()) |argument| {
            self.argument1 = argument;
            if (argument.len >= 2 and std.mem.eql(u8, "//", argument[0..2])) {
                return false;
            }
            self.command_type = self.argument1_table.get(argument) orelse {
                std.debug.print("{s} {s}\n", .{ self.command.?, argument });
                unreachable;
            };
        }

        self.argument2 = 0;
        while (arguments_iterator.next()) |argument| {
            if (2 <= argument.len and std.mem.eql(u8, "//", argument[0..2])) {
                break;
            }
            self.argument2 += 1;
        }

        switch (self.command_type.?) {
            .C_LABEL, .C_GOTO, .C_IF => if (1 != self.argument2) {
                unreachable;
            },
            .C_ARITHMETIC, .C_RETURN => if (0 != self.argument2) {
                unreachable;
            },
            .C_FUNCTION, .C_CALL, .C_PUSH, .C_POP => if (2 != self.argument2) {
                unreachable;
            },
        }

        return true;
    }

    pub fn init(allocator: std.mem.Allocator) !Self {
        var self: Self = Self{
            .allocator = allocator,
            .command_iterator = null,
            .content = null,
            .command = null,
            .command_type = null,
            .argument1 = null,
            .argument2 = 0,
            .argument1_table = undefined,
            .content_table = undefined,
            .file = "",
        };

        self.content_table = std.StringHashMap([]u8).init(self.allocator);
        self.argument1_table = std.StringHashMap(CommandType).init(self.allocator);
        try self.argument1_table.put("add", .C_ARITHMETIC);
        try self.argument1_table.put("sub", .C_ARITHMETIC);
        try self.argument1_table.put("neg", .C_ARITHMETIC);
        try self.argument1_table.put("eq", .C_ARITHMETIC);
        try self.argument1_table.put("gt", .C_ARITHMETIC);
        try self.argument1_table.put("lt", .C_ARITHMETIC);
        try self.argument1_table.put("and", .C_ARITHMETIC);
        try self.argument1_table.put("or", .C_ARITHMETIC);
        try self.argument1_table.put("not", .C_ARITHMETIC);
        try self.argument1_table.put("push", .C_PUSH);
        try self.argument1_table.put("pop", .C_POP);
        try self.argument1_table.put("label", .C_LABEL);
        try self.argument1_table.put("goto", .C_GOTO);
        try self.argument1_table.put("if-goto", .C_IF);
        try self.argument1_table.put("function", .C_FUNCTION);
        try self.argument1_table.put("call", .C_CALL);
        try self.argument1_table.put("return", .C_RETURN);

        return self;
    }

    pub fn openFileAbsolute(self: *Self, file: []const u8) !void {
        if (self.content_table.get(file)) |content| {
            self.file = self.content_table.getKey(file).?;
            self.content = content;
            self.command_iterator = std.mem.tokenizeAny(u8, self.content.?, "\r\n");
            return;
        }

        var input: std.fs.File = try std.fs.openFileAbsolute(file, .{});
        self.content = try input.readToEndAlloc(self.allocator, try input.getEndPos());
        self.file = try std.fmt.allocPrint(self.allocator, "{s}", .{file});
        try self.content_table.put(self.file, self.content.?);
        var iter = std.mem.tokenizeAny(u8, file, "\\/");
        var temp_file: []const u8 = "";
        while (iter.next()) |n| {
            temp_file = n;
        }
        self.file = try std.fmt.allocPrint(self.allocator, "{s}", .{temp_file});
        self.command_iterator = std.mem.tokenizeAny(u8, self.content.?, "\r\n");
    }

    pub fn openFile(self: *Self, file: []const u8) !void {
        if (self.content_table.get(file)) |content| {
            self.file = self.content_table.getKey(file).?;
            self.content = content;
            self.command_iterator = std.mem.tokenizeAny(u8, self.content.?, "\r\n");
            return;
        }

        var input: std.fs.File = try std.fs.cwd().openFile(file, .{});
        self.content = try input.readToEndAlloc(self.allocator, try input.getEndPos());
        var iter = std.mem.tokenizeAny(u8, file, "\\/");
        var temp_file: []const u8 = "";
        while (iter.next()) |n| {
            temp_file = n;
        }
        self.file = try std.fmt.allocPrint(self.allocator, "{s}", .{temp_file});
        try self.content_table.put(self.file, self.content.?);
        self.command_iterator = std.mem.tokenizeAny(u8, self.content.?, "\r\n");
    }

    pub fn closeFile(self: *Self) void {
        self.content = null;
        self.command_iterator = null;
        self.content = null;
        self.command_type = null;
        self.command = null;
        self.argument1 = null;
        self.argument2 = 0;
    }

    pub fn hasMoreCommands(self: *Self) bool {
        if (self.command_iterator) |*iterator| {
            if (iterator.next()) |command| {
                self.command = command;
                if (!self.countArguments())
                    return self.hasMoreCommands();
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    pub fn advance(self: *Self) bool {
        return self.hasMoreCommands();
    }

    pub fn reparse(self: *Self) void {
        if (self.command_iterator) |iterator| {
            iterator.reset();
            self.command = null;
            self.command_type = null;
            self.command_type = null;
            self.argument1 = null;
            self.argument2 = 0;
        }
    }

    pub fn commandType(self: Self) ?CommandType {
        return self.command_type;
    }

    pub fn deinit(self: *Self) void {
        var iter = self.content_table.keyIterator();
        while (iter.next()) |key| {
            self.allocator.free(self.content_table.get(key.*).?);
            self.allocator.free(key.*);
        }
        self.argument1_table.deinit();
    }
};
