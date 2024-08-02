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
    function: []const u8,
    function_table: std.StringHashMap(i32),
    function_call: std.StringHashMap(i32),

    const Self = @This();

    pub fn init(allocator: std.mem.Allocator) !Self {
        var self = Self{
            .output = null,
            .allocator = allocator,
            .lt_count = 0,
            .gt_count = 0,
            .eq_count = 0,
            .arithmetic_table = undefined,
            .function = "",
            .function_table = undefined,
            .function_call = undefined,
        };

        self.function_call = std.StringHashMap(i32).init(self.allocator);
        self.function_table = std.StringHashMap(i32).init(self.allocator);
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
            \\@Sys.init
            \\0; JMP
            \\
        );
    }

    pub fn deinit(self: *Self) void {
        if (self.output) |output| {
            output.close();
        }
        self.arithmetic_table.deinit();
        self.function_call.deinit();
        self.function_table.deinit();
    }

    pub fn wrtieArithmetic(self: *Self, _command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, _command, " \t");
        const command: []const u8 = iterator.next().?;
        if (std.mem.eql(u8, "lt", command)) {
            try self.output.?.writer().print(lt, .{self.lt_count});
            self.lt_count += 1;
            return;
        } else if (std.mem.eql(u8, "gt", command)) {
            try self.output.?.writer().print(gt, .{self.gt_count});
            self.gt_count += 1;
            return;
        } else if (std.mem.eql(u8, "eq", command)) {
            try self.output.?.writer().print(eq, .{self.eq_count});
            self.eq_count += 1;
            return;
        }

        if (self.arithmetic_table.get(command)) |con| {
            try self.output.?.writeAll(con);
        } else {
            std.debug.print("command: {s}\n", .{command});
            unreachable;
        }
    }

    pub fn writePushPop(self: *Self, command: []const u8, file: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
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
                    \\@{s}.static.{}
                    \\D = M
                    \\@SP
                    \\M = M + 1
                    \\A = M - 1
                    \\M = D
                    \\
                , .{ file, pos });
            } else {
                try self.output.?.writer().print(
                    \\@SP
                    \\M = M - 1
                    \\A = M
                    \\D = M
                    \\@{s}.static.{}
                    \\M = D
                    \\
                , .{ file, pos });
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

    pub fn writeLabel(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const label: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };

        if (self.output) |output| {
            try output.writer().print("({s}${s})\n", .{ self.function, label });
        }
    }

    pub fn writeGoto(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const label: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };

        if (self.output) |output| {
            try output.writer().print(
                \\@{s}${s}
                \\0; JMP
                \\
            , .{ self.function, label });
        }
    }

    pub fn writeIf(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const label: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };

        if (self.output) |output| {
            try output.writer().print(
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@{s}${s}
                \\D; JNE
                \\
            , .{ self.function, label });
        }
    }

    pub fn recoredFunction(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const function: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };
        const lcl: i32 = try std.fmt.parseInt(i32, iterator.next().?, 10);

        try self.function_table.put(function, lcl);
    }

    pub fn writeFunction(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const function: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };

        if (self.output) |output| {
            try output.writer().print("({s})\n", .{function});
        }

        self.function = function;
        self.function_call.clearRetainingCapacity();
    }

    pub fn writeCall(self: *Self, command: []const u8) !void {
        var iterator: std.mem.TokenIterator(u8, .any) = std.mem.tokenizeAny(u8, command, " \t");
        const function: []const u8 = blk: {
            _ = iterator.next();
            break :blk iterator.next().?;
        };
        const arg: i32 = try std.fmt.parseInt(i32, iterator.next().?, 10);
        const lcl: i32 = self.function_table.get(function) orelse {
            std.debug.print("function {s} was not declared\n", .{function});
            unreachable;
        };
        var time: i32 = 0;
        if (self.function_call.get(function)) |t| {
            time = t;
        }
        try self.function_call.put(function, time + 1);

        if (self.output) |output| {
            try output.writer().print(
                \\@ARG
                \\D = M
                \\@R13
                \\M = D
                \\@SP
                \\D = M
                \\@{}
                \\D = D - A
                \\@ARG
                \\M = D
                \\
            , .{arg});

            try output.writer().print(
                \\@{s}.call.{s}.times.{}.end
                \\D = A
                \\@SP
                \\M = M + 1
                \\A = M - 1
                \\M = D
                \\@LCL
                \\D = M
                \\@SP
                \\M = M + 1
                \\A = M - 1
                \\M = D
                \\@R13
                \\D = M
                \\@SP
                \\M = M + 1
                \\A = M - 1
                \\M = D
                \\@THIS
                \\D = M
                \\@SP
                \\M = M + 1
                \\A = M - 1
                \\M = D
                \\@THAT
                \\D = M
                \\@SP
                \\M = M + 1
                \\A = M - 1
                \\M = D
                \\@SP
                \\D = M
                \\@LCL
                \\M = D
                \\
            , .{ self.function, function, time });

            for (0..@intCast(lcl)) |_| {
                try self.writePushPop("push constant 0", "");
            }

            try output.writer().print(
                \\@{s}
                \\0; JMP
                \\({s}.call.{s}.times.{}.end)
                \\
            , .{ function, self.function, function, time });
        }
    }

    pub fn writeReturn(self: *Self) !void {
        if (self.output) |output| {
            try output.writeAll(
                \\@SP
                \\A = M - 1
                \\D = M
                \\@R13
                \\M = D
                \\@LCL
                \\D = M
                \\@SP
                \\M = D
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@THAT
                \\M = D
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@THIS
                \\M = D
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@R14
                \\M = D
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@LCL
                \\M = D
                \\@SP
                \\M = M - 1
                \\A = M
                \\D = M
                \\@R15
                \\M = D
                \\@ARG
                \\D = M
                \\@SP
                \\M = D + 1
                \\@R13
                \\D = M
                \\@SP
                \\A = M - 1
                \\M = D
                \\@R14
                \\D = M
                \\@ARG
                \\M = D
                \\@R15
                \\A = M
                \\0; JMP
                \\
            );
        }
    }
};
