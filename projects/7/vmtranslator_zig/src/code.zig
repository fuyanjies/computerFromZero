const std = @import("std");

const gt: []const u8 =
    \\@SP
    \\M = M - 1
    \\A = M
    \\D = M
    \\@SP
    \\A = M - 1
    \\D = M - D
    \\@gt.true.{0}
    \\D; JGT
    \\@SP
    \\A = M - 1
    \\M = 0
    \\@gt.end.{0}
    \\0; JMP
    \\(gt.true.{0})
    \\@SP
    \\A = M - 1
    \\M = -1
    \\(gt.end.{0})
    \\
;
const eq: []const u8 =
    \\@SP
    \\M = M - 1
    \\A = M
    \\D = M
    \\@SP
    \\A = M - 1
    \\D = M - D
    \\@eq.true.{0}
    \\D; JEQ
    \\@SP
    \\A = M - 1
    \\M = 0
    \\@eq.end.{0}
    \\0; JMP
    \\(eq.true.{0})
    \\@SP
    \\A = M - 1
    \\M = -1
    \\(eq.end.{0})
    \\
;
const lt: []const u8 =
    \\@SP
    \\M = M - 1
    \\A = M
    \\D = M
    \\@SP
    \\A = M - 1
    \\D = M - D
    \\@lt.true.{0}
    \\D; JLT
    \\@SP
    \\A = M - 1
    \\M = 0
    \\@lt.end.{0}
    \\0; JMP
    \\(lt.true.{0})
    \\@SP
    \\A = M - 1
    \\M = -1
    \\(lt.end.{0})
    \\
;

pub const Code = struct {
    output: ?std.fs.File,
    allocator: std.mem.Allocator,
    lt_count: usize,
    gt_count: usize,
    eq_count: usize,
    arithmetic_table: std.StringHashMap([]const u8),

    const Self = @This();

    pub fn init(allocator: std.mem.Allocator) !Self {
        var self = Self{
            .output = null,
            .allocator = allocator,
            .lt_count = 0,
            .gt_count = 0,
            .eq_count = 0,
            .arithmetic_table = undefined,
        };

        self.arithmetic_table = std.StringHashMap([]const u8).init(self.allocator);
        try self.arithmetic_table.put("add",
            \\@SP
            \\M = M - 1
            \\A = M
            \\D = M
            \\@SP
            \\A = M - 1
            \\M = D + M
            \\
        );
        try self.arithmetic_table.put("sub",
            \\@SP
            \\M = M - 1
            \\A = M
            \\D = M
            \\@SP
            \\A = M - 1
            \\M = M - D
            \\
        );
        try self.arithmetic_table.put("neg",
            \\@SP
            \\A = M - 1
            \\M = -M
            \\
        );
        try self.arithmetic_table.put("and",
            \\@SP
            \\M = M - 1
            \\A = M
            \\D = M
            \\@SP
            \\A = M - 1
            \\M = M & D
            \\
        );
        try self.arithmetic_table.put("or",
            \\@SP
            \\M = M - 1
            \\A = M
            \\D = M
            \\@SP
            \\A = M - 1
            \\M = M | D
            \\
        );
        try self.arithmetic_table.put("not",
            \\@SP
            \\A = M - 1
            \\M = !M
            \\
        );

        return self;
    }

    pub fn setFileName(self: *Self, file_name: []const u8) !void {
        if (self.output) |_| {
            return error.HasStarted;
        }
        if ('/' == file_name[0] or ':' == file_name[1]) {
            self.output = try std.fs.createFileAbsolute(file_name, .{});
        } else {
            self.output = try std.fs.cwd().createFile(file_name, .{});
        }

        try self.output.?.writeAll(
            \\@256
            \\D = A
            \\@SP
            \\M = D
            \\
        );
    }

    pub fn deinit(self: *Self) void {
        if (self.output) |output| {
            output.close();
        }
        self.arithmetic_table.deinit();
    }

    pub fn wrtieArithmetic(self: *Self, command: []const u8) !void {
        if (null == self.output) {
            unreachable;
        }

        if (std.mem.eql(u8, "lt", command)) {
            // std.debug.print(lt, .{self.lt_count});
            try self.output.?.writer().print(lt, .{self.lt_count});
            self.lt_count += 1;
            return;
        } else if (std.mem.eql(u8, "gt", command)) {
            // std.debug.print(gt, .{self.gt_count});
            try self.output.?.writer().print(gt, .{self.gt_count});
            self.gt_count += 1;
            return;
        } else if (std.mem.eql(u8, "eq", command)) {
            // std.debug.print(eq, .{self.eq_count});
            try self.output.?.writer().print(eq, .{self.eq_count});
            self.eq_count += 1;
            return;
        }

        if (self.arithmetic_table.get(command)) |con| {
            // std.debug.print("{s}\n", .{con});
            try self.output.?.writeAll(con);
        } else {
            std.debug.print("command: {s}\n", .{command});
            unreachable;
        }
    }

    pub fn writePushPop(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .scalar) = std.mem.tokenizeScalar(u8, command, ' ');
        var is_push: bool = undefined;

        if (iterator.next()) |first| {
            if (std.mem.eql(u8, "push", first)) {
                is_push = true;
            } else {
                is_push = false;
            }
        }

        const segment: []const u8 = iterator.next().?;
        const pos: i32 = std.fmt.parseInt(i32, iterator.next().?, 10) catch |err| {
            std.debug.print("ERROR: {?}\n", .{err});
            unreachable;
        };

        if (std.mem.eql(u8, "constant", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@{0}
                    \\D = A
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos});
            } else {
                unreachable;
            }
        } else if (std.mem.eql(u8, "pointer", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@{0}
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos + 3});
            } else {
                try self.output.?.writer().print(
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@{0}
                    \\M = D
                    \\
                , .{pos + 3});
            }
        } else if (std.mem.eql(u8, "temp", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@{0}
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos + 5});
            } else {
                try self.output.?.writer().print(
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@{0}
                    \\M = D
                    \\
                , .{pos + 5});
            }
        } else if (std.mem.eql(u8, "local", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@LCL
                    \\D = M
                    \\@{0}
                    \\A = D + A
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos});
            } else {
                try self.output.?.writer().print(
                    \\@LCL
                    \\D = M
                    \\@{0}
                    \\D = D + A
                    \\@R13
                    \\M = D
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@R13
                    \\A = M
                    \\M = D
                    \\
                , .{pos});
            }
        } else if (std.mem.eql(u8, "argument", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@ARG
                    \\D = M
                    \\@{0}
                    \\A = D + A
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos});
            } else {
                try self.output.?.writer().print(
                    \\@ARG
                    \\D = M
                    \\@{0}
                    \\D = D + A
                    \\@R13
                    \\M = D
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@R13
                    \\A = M
                    \\M = D
                    \\
                , .{pos});
            }
        } else if (std.mem.eql(u8, "static", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@{0}
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos + 16});
            } else {
                try self.output.?.writer().print(
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@{0}
                    \\M = D
                    \\
                , .{pos + 16});
            }
        } else if (std.mem.eql(u8, "this", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@THIS
                    \\D = M
                    \\@{0}
                    \\A = D + A
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos});
            } else {
                try self.output.?.writer().print(
                    \\@THIS
                    \\D = M
                    \\@{0}
                    \\D = D + A
                    \\@R13
                    \\M = D
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@R13
                    \\A = M
                    \\M = D
                    \\
                , .{pos});
            }
        } else if (std.mem.eql(u8, "that", segment)) {
            if (is_push) {
                try self.output.?.writer().print(
                    \\@THAT
                    \\D = M
                    \\@{0}
                    \\A = D + A
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{pos});
            } else {
                try self.output.?.writer().print(
                    \\@THAT
                    \\D = M
                    \\@{0}
                    \\D = D + A
                    \\@R13
                    \\M = D
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@R13
                    \\A = M
                    \\M = D
                    \\
                , .{pos});
            }
        }
    }
};
