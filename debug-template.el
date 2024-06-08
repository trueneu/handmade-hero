(dap-register-debug-template
  "localhost:12345"
  (list :type "cppdbg"
        :request "launch"
        :name "cpptools::Run Configuration"
        :MIMode "gdb"
        :MIDebuggerServerAddress "localhost:12345"
        :program "${workspaceFolder}/build/handmade_win32.exe"
        :cwd "${workspaceFolder}"))
