# frozen_string_literal: true

require 'sinatra'
require 'sinatra/namespace'

require './serial_port_handler.rb'

set :server, %w[thin puma webrick]

sp_handler = SerialPortHandler.new('COM2')

get '/' do
  erb :index
end

namespace '/api' do
  before do
    content_type :json
    if request.post?
      request.body.rewind
      @data = JSON.parse(request.body.read)
    end
  end

  get '/read' do
    { commands: sp_handler.read }
  end

  post '/write' do
    str = @data['command']
    sp_handler.write(str)
    return
  end

  after do
    response.body = JSON.dump(response.body)
  end
end
