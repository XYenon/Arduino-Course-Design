# frozen_string_literal: true

require 'rubyserial'

class SerialPortHandler
  def initialize(port = 'COM1')
    @serial_port = Serial.new(port)
  end

  def write(str)
    @serial_port.write(str + '|')
  end

  def read(len = 128)
    str = @serial_port.read(len)
    str << @serial_port.read(1) until str.empty? || str.end_with?('|')
    str.strip.split('|').map(&:strip)
  end
end
