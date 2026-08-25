spoon.uuid64 = {}
spoon.uuid64.__index = spoon.uuid64

math.randomseed(os.time())

--- Generates a unique 64-bit integer
-- Structure: [32 bits: Timestamp] + [32 bits: Random Entropy]
local function generate_int64()
    local time_part = os.time() & 0xFFFFFFFF
    -- generate two 16-bit random numbers to get a full 32-bit random integer
    local rand_high = math.random(0, 0xFFFF)
    local rand_low = math.random(0, 0xFFFF)
    local rand_part = (rand_high << 16) | rand_low
    
    -- combine them into a single 64-bit integer
    return (time_part << 32) | rand_part
end

function spoon.uuid64.new(existing_val)
    local self = setmetatable({}, spoon.uuid64)
    
    if existing_val then
        self.value = math.tointeger(existing_val) or error("Invalid integer passed")
    else
        self.value = generate_int64()
    end
    
    return self
end

function spoon.uuid64:to_number()
    return self.value
end

function spoon.uuid64:to_hex()
    return string.format("%016x", self.value)
end

function spoon.uuid64.from_string(str)
    if type(str) ~= "string" then
        error("from_string: expected a string, but got: " .. type(str))
    end

    local parsed_int = nil

    if string.match(str, "^0[xX]") then
        parsed_int = math.tointeger(str)
    
    elseif string.match(str, "^%x+$") and #str == 16 then
        parsed_int = math.tointeger("0x" .. str)
    
    elseif string.match(str, "^%d+$") then
        parsed_int = math.tointeger(str)
        
        if not parsed_int then
            local high = 0
            local low = 0
            
            for i = 1, #str do
                local digit = tonumber(string.sub(str, i, i))
                
                local low_next = low * 10 + digit
                local high_next = high * 10 + math.floor(low_next / 4294967296)
                
                low = low_next % 4294967296
                high = high_next % 4294967296
            end
            
            parsed_int = (high << 32) | low
        end
    end

    if not parsed_int then
        error("Can not convert the string '" .. str .. "' into a 64-bit integer")
    end

    return spoon.uuid64.new(parsed_int)
end

function spoon.uuid64:__tostring()
    -- format as an unsigned 64-bit integer string
    return string.format("%u", self.value)
end

function spoon.uuid64:__eq(other)
    if getmetatable(other) == spoon.uuid64 then
        return self.value == other.value
    end
    
    if type(other) == "string" then
        if string.match(other, "^0[xX]") or string.match(other, "^%x+$") and #other == 16 then
            return self:to_hex() == string.lower(string.gsub(other, "^0[xX]", ""))
        end
        
        local num = math.tointeger(other)
        if num then
            return self.value == num
        end
    end
    
    return false
end

return spoon.uuid64
