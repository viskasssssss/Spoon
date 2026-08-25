name = "World"
spoon.print_rawf("Hello, %s!", name);

events.on("app.on_ui_render", function()
    spoon.ui.start(spoon.window.get_main())

    spoon.ui.window("Plugin test")

    spoon.ui.text("Hello I'm plugin window 1")

    spoon.ui.pop(1)

    spoon.ui.window("Plugin test 2!")

    spoon.ui.textf("Hello, %s!", name)

    spoon.ui.pop(1)
end)