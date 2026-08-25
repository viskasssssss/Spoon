spoon = spoon or {}

spoon.ui = {}
spoon.window = {}

function spoon.print_raw(text)
    spoon._internal.call(101, "Hello, World!")
end

function spoon.print_rawf(format_string, ...)
    local formatted_text = string.format(format_string, ...)

    spoon._internal.call(101, formatted_text)
end

function spoon.ui.start(id)
    spoon._internal.call(201, id)
end

function spoon.ui.window(title)
    spoon._internal.call(203, title)
end

function spoon.ui.pop(amount)
    for i = 1, amount do
        spoon._internal.call(202)
    end
end

function spoon.ui.text(text)
    spoon._internal.call(204, "text", text)
end

function spoon.ui.textf(format_string, ...)
    local formatted_text = string.format(format_string, ...)

    spoon._internal.call(204, "text", formatted_text)
end

function spoon.window.get_main()
    return spoon._internal.call(301)
end

