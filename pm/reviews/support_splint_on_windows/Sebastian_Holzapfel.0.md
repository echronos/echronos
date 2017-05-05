Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Location: prj/app/lib/util/util.py:231
Comment 1: Rework
Everything works fine on linux & windows (native), but I have issues when running in a cygwin environment:

    $ prj/app/prj.py analyze stub.acamar
    ERROR:prj:An unhandled exception occurred: Unsupported platform cygwin
    ERROR:prj:Please include the 'prj.errors' file when submitting a bug report
    $ cat prj.errors
    Traceback (most recent call last):
      File "prj/app/prj.py", line 1302, in _start
        sys.exit(main())
      File "prj/app/prj.py", line 1294, in main
        return SUBCOMMAND_TABLE[args.command](args)
      File "prj/app/prj.py", line 1131, in analyze
        return call_system_function(args, System.analyze)
      File "prj/app/prj.py", line 1165, in call_system_function
        prepend_tool_binaries_to_path_environment_variable()
      File "prj/app/lib/util/util.py", line 216, in prepend_tool_binaries_to_path_environment_variable
        for tool_path in _get_platform_tool_paths():
      File "prj/app/lib/util/util.py", line 231, in _get_platform_tool_paths
        raise RuntimeError('Unsupported platform {}'.format(sys.platform))
    RuntimeError: Unsupported platform cygwin

Looks like `sys.platform` is returning a string that doesn't have a tool path mapping.
