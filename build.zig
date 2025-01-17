const std = @import("std");
const libiup = @import("cpank/ext/iup/build.zig");

pub fn build(b: *std.Build) void {
    
    //const outputDir = "build/";
    

    const cflags = [_][]const u8{
        "-Wall",
        "-std=c11"
    };

    const sourceToRun = "sample/0.pank";

    //Core of Cpank
    var srcCore = [_][]const u8{
        "cpank/api.c",
        "cpank/bn.c",
        "cpank/builtins.c",
        "cpank/compiler.c",
        "cpank/debug.c",
        "cpank/errmsg.c",
        "cpank/htable.c",
        "cpank/instruction.c",
        "cpank/lexer.c",
        "cpank/mem.c",
        "cpank/obj.c",
        "cpank/openfile.c",
        "cpank/runfile.c",
        "cpank/utils.c",
        "cpank/value.c",
        "cpank/vm.c",
        "cpank/main.c",
    };

    //Standard Library
    var srcStdlib = [_][]const u8{
        "cpank/stdlib/array.c",
        "cpank/stdlib/big.c",
        "cpank/stdlib/bnmath.c",
        "cpank/stdlib/bnos.c",
        "cpank/stdlib/common.c",
        "cpank/stdlib/file.c",
        "cpank/stdlib/map.c",
        "cpank/stdlib/math.c",
        "cpank/stdlib/os.c",
        "cpank/stdlib/stdlib.c",
        "cpank/stdlib/string.c",
    };

    //External Library
    var srcExt = [_][]const u8{
        "cpank/ext/baurinum/absneg.c",
        "cpank/ext/baurinum/addsub.c",
        "cpank/ext/baurinum/baurinum.c",
        "cpank/ext/baurinum/compare.c",
        "cpank/ext/baurinum/getter.c",
        "cpank/ext/baurinum/setter.c",
        "cpank/ext/baurinum/utility.c",
    };

    var srcGui = [_][]const u8{
        "gui/gui.c",
    };

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{ .name = "pankti", .target = target, .optimize = optimize });
    const lib = b.addStaticLibrary(.{ .name = "pankti", .optimize = optimize, .target = target });
    const guiexe = b.addExecutable(.{ .name = "panktiw", .target = target, .optimize = optimize });
    
    

    guiexe.addCSourceFiles(&srcGui, &.{});
    
    exe.linkLibC();
    guiexe.linkLibC();
    lib.linkLibC();

    exe.defineCMacro("MODE_BENGALI", "");
    lib.defineCMacro("MODE_BENGALI", "");
    lib.defineCMacro("APILIB", "");
    guiexe.defineCMacro("MODE_BENGALI", "");
    guiexe.defineCMacro("APILIB", "lib");

    //CORE
    exe.addCSourceFiles(&srcCore, &cflags);
    lib.addCSourceFiles(&srcCore, &cflags);

    //STDLIB
    exe.addCSourceFiles(&srcStdlib, &cflags);
    lib.addCSourceFiles(&srcStdlib, &cflags);

    //lib.addCSourceFiles(files: []const []const u8, flags: []const []const u8)

    //External
    exe.addCSourceFiles(&srcExt, &cflags);
    lib.addCSourceFiles(&srcExt, &cflags);

    exe.linkSystemLibrary("m");
    guiexe.linkLibrary(lib);
    const ui = libiup.addIup(b, target, std.builtin.Mode.ReleaseFast);
    guiexe.linkLibrary(ui);
    
    

    //Build Executable Command
    const exe_build = b.step("x", "Build Executable 'pankti'");
    const install_exe = b.addInstallArtifact(exe);
    //exe.setOutputDir("build");
    //Build GUI Executable
    const gui_exe_build = b.step("g", "Build Gui 'panktiw'");
    const install_gui_exe = b.addInstallArtifact(guiexe);
    exe_build.dependOn(&install_exe.step);
    gui_exe_build.dependOn(&install_gui_exe.step);
    
    //Build Static Library
    const lib_build = b.step("lib", "Build Static Library 'libpankti.a'");

    const install_lib = b.addInstallArtifact(lib);
    lib_build.dependOn(&install_lib.step);
    

    const run_cmd_run = b.addSystemCommand(&[_][]const u8{
        "zig-out/bin/pankti",
        sourceToRun,
    });

    const run_cmd = b.step("run", "Build and Run Sample");
    run_cmd.dependOn(&exe.step);
    run_cmd.dependOn(&run_cmd_run.step);

    b.installArtifact(exe);

    

    

}
