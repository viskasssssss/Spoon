spoon = spoon or {}
events = events or {}

spoon.ui = {}
spoon.window = {}

spoon.ui._buttons = {}
spoon.ui._button_index = 0
spoon.ui._button_ids = {}

events.on("ui.button", function(btn_id)
    local key = tostring(btn_id)

    if spoon.ui._buttons[key] then
        spoon.ui._buttons[key].pressed = true
    end
end)

function spoon.ui.button(text)
    spoon.ui._button_index = spoon.ui._button_index + 1

    local index = spoon.ui._button_index

    if spoon.ui._button_ids[index] == nil then
        spoon.ui._button_ids[index] = spoon.uuid64.new()
    end

    local button_id = spoon.ui._button_ids[index]
    local key = tostring(button_id)

    if spoon.ui._buttons[key] == nil then
        spoon.ui._buttons[key] = {
            id = button_id,
            pressed = false
        }
    end

    spoon._internal.call(204, "button", text, button_id)

    return button_id
end

function spoon.ui.buttonf(format_string, ...)
    local formatted_text = string.format(format_string, ...)

    return spoon.ui.button(formatted_text)
end

function spoon.ui.button_result(button_id)
    if button_id == nil then
        return false
    end

    local key = tostring(button_id)
    local button = spoon.ui._buttons[key]

    if button == nil then
        return false
    end

    if button.pressed then
        button.pressed = false
        return true
    end

    return false
end


function spoon.print_raw(text)
    spoon._internal.call(101, text)
end

function spoon.print_rawf(format_string, ...)
    local formatted_text = string.format(format_string, ...)

    spoon._internal.call(101, formatted_text)
end

function spoon.ui.start(id)
    local converted = tostring(id)

    spoon.ui._button_index = 0

    spoon._internal.call(201, converted)
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
    local id = spoon._internal.call(301)
    local converted = spoon.uuid64.from_string(id)
    return converted
end

