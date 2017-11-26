client = MEES::TCPClient.new("192.168.1.100", 4567)
client.puts "Hello"

pin = ESP32::GPIO::Pin.new(23, :inout)

MEES.interval 1 do
  pin.toggle
end

MEES.main do
  if data=client.recv_nonblock  
    puts data
  end
end
