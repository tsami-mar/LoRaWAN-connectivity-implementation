function parsePayload(appeui, deveui, payload) 
    payload = resiot_hexdecode(payload)
    local pos = 1

 -- Colours
    local tag_clear = "ClearValue"
    local clear = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    resiot_setnodevalue(appeui, deveui, tag_clear, clear)
    resiot_debug("Parsed Clear Value: " .. clear) -- Debug 
    pos = pos + 2

    local tag_red = "RedValue"
    local red = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    resiot_setnodevalue(appeui, deveui, tag_red, red)
    resiot_debug("Parsed Red Value: " .. red) -- Debug 
    pos = pos + 2

    local tag_green = "GreenValue"
    local green = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    resiot_setnodevalue(appeui, deveui, tag_green, green)
    resiot_debug("Parsed Green Value: " .. green) -- Debug 
    pos = pos + 2

    local tag_blue = "BlueValue"
    local blue = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    resiot_setnodevalue(appeui, deveui, tag_blue, blue)
    resiot_debug("Parsed Blue Value: " .. blue) -- Debug 
    pos = pos + 2

-- Accelerometer
    local tag_accel_x = "AccelX"
    local raw_x = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    -- Adjust for signed 14-bit range
    if raw_x > (2^14 / 2 - 1) then
        raw_x = raw_x - (2^14) -- Adjust for signed 14-bit range
    end
    -- Scale and convert to m/s²
    local accel_x = (raw_x / 4096.0) * 9.81
  	accel_x = tonumber(string.format("%.2f", accel_x))
    resiot_setnodevalue(appeui, deveui, tag_accel_x, accel_x)
    resiot_debug("Parsed Accel X Value: " .. accel_x) -- Debug 
    pos = pos + 2

  
    local tag_accel_y = "AccelY"
    local raw_y = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    -- Adjust for signed 14-bit range
    if raw_y > (2^14 / 2 - 1) then
        raw_y = raw_y - (2^14) -- Adjust for signed 14-bit range
    end
    -- Scale and convert to m/s²
    local accel_y = (raw_y / 4096.0) * 9.81	
  	accel_y = tonumber(string.format("%.2f", accel_y))
    resiot_setnodevalue(appeui, deveui, tag_accel_y, accel_y)
    resiot_debug("Parsed Accel Y Value: " .. accel_y) -- Debug 
    pos = pos + 2

  
    local tag_accel_z = "AccelZ"
  	local raw_z =  resiot_ba2intLE16({payload[pos], payload[pos + 1]})
    -- Adjust for signed 14-bit range
    if raw_z > (2^14 / 2 - 1) then
        raw_z = raw_z - (2^14) -- Adjust for signed 14-bit range
    end
    -- Scale and convert to m/s²
    local accel_z = (raw_z / 4096.0) * 9.81
  	accel_z = tonumber(string.format("%.2f", accel_z))
    resiot_setnodevalue(appeui, deveui, tag_accel_z, accel_z)
  	resiot_debug("Parsed Accel Z Value: " .. accel_z) -- Debug 
    pos = pos + 2

  
 --Light 
  	local tag_light = "Light"
    local light = resiot_ba2intLE16({payload[pos], payload[pos + 1]}) /10
    resiot_setnodevalue(appeui, deveui, tag_light, light)
    resiot_debug("Parsed Light Value: " .. light) -- Debug 
    pos = pos + 2
 
 --Moisture 
  	local tag_moisture = "Moisture"
    local moisture = resiot_ba2intLE16({payload[pos], payload[pos + 1]}) /10
    resiot_setnodevalue(appeui, deveui, tag_moisture, moisture)
    resiot_debug("Parsed Moisture Value: " .. moisture) -- Debug
  	pos = pos + 2
  
 --Temperature 
  	local tag_temp = "Temperature"
    local raw_temp = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
  	local temp = (175.72 * raw_temp) / 65536.0 - 46.85
  	temp = tonumber(string.format("%.1f", temp))
    resiot_setnodevalue(appeui, deveui, tag_temp, temp)
    resiot_debug("Parsed Temperature Value: " .. temp) -- Debug 
    pos = pos + 2
 
 --Humidity 
  	local tag_humidity = "Humidity"
    local raw_humidity = resiot_ba2intLE16({payload[pos], payload[pos + 1]})
	local humidity = (125.0 * raw_humidity) / 65536.0 - 6.0
  	humidity = tonumber(string.format("%.1f", humidity))
    resiot_setnodevalue(appeui, deveui, tag_humidity, humidity)
    resiot_debug("Parsed Humidity Value: " .. humidity) -- Debug
  	pos = pos + 2
  
 --Latitude 
  	local tag_latitude = "Latitude"
    local latitude = resiot_ba2float32LE({payload[pos], payload[pos + 1], payload[pos + 2], payload[pos + 3]})
    resiot_setnodevalue(appeui, deveui, tag_latitude, latitude)
    resiot_debug(string.format("Parsed Latitude: " .. latitude)) -- Debug
	pos = pos + 4
  
 --Longitude 
  	local tag_longitude = "Longitude"
    local longitude = resiot_ba2float32LE({payload[pos], payload[pos + 1], payload[pos + 2], payload[pos + 3]})
    resiot_setnodevalue(appeui, deveui, tag_longitude, longitude)
	resiot_debug(string.format("Parsed Longitude: " .. longitude)) -- Debug

end

Origin = resiot_startfrom() --Scene process starts here

if Origin == "Manual" then
  -- Manual script execution for testing
  -- Set your test payload here in hexadecimal
  payload = "0028002800280028dc3fc8001a10e0037c00c463e26ef931b5c121702242"
  -- Set your Application EUI here
  appeui = "70b3d57ed000fc4d"
  -- Set your own Device EUI here
  deveui = "4039323559379194"
else
  -- Normal execution, get payload received from device
  appeui = resiot_comm_getparam("appeui")
  deveui = resiot_comm_getparam("deveui")
  payload, err = resiot_getlastpayload(appeui, deveui)
  resiot_debug("SN_TEST_GG Test Auto Mode\n")
end
-- Do your stuff
parsePayload(appeui,deveui,payload)