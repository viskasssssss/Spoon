local world = "World"


events.on("app.on_ui_render", function()
    spoon.ui.start(spoon.window.get_main())

    spoon.ui.window("Plugin test")

    spoon.ui.text("Hello I'm plugin window 1")

    local button_id = spoon.ui.button("button 1")

    if (spoon.ui.button_result(button_id)) then
        spoon.print_raw("Button 1 pressed")
    end

    button_id = spoon.ui.button("button 2")

    if (spoon.ui.button_result(button_id)) then
        spoon.print_raw("Button 2 pressed")
    end

    spoon.ui.pop(1)

    spoon.ui.window("Plugin test 2!")

    spoon.ui.textf("Hello, %s!", world)

    button_id = spoon.ui.buttonf("%s", "Hello, World!");

    if (spoon.ui.button_result(button_id)) then
        spoon.print_raw("Button 'Hello, World!' pressed")
    end

    spoon.ui.pop(1)
end)