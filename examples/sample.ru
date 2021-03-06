# -*- coding: utf-8 -*-

# Usage:
#  $ sudo macgem install control_tower
#  $ control_tower -R sample.ru

framework 'Foundation'
require 'rack'
require 'rubygems'
require 'system_control'

SERVER_NAME = ""
SERVER_PORT = 3000

HTML =<<END
<html>
<head>
<style type="text/css">
form { display: inline; }
</style>
</head>
<body>

System:
<form action="/sleep"><input type="submit" value="sleep"></form>

<hr>

Volume:
<form action="/vol_up"><input type="submit" value="+"></form>
<form action="/vol_down"><input type="submit" value="-"></form>

<hr>

App:
<form action="/app_itunes"><input type="submit" value="launch iTunes"></form>

</body>
</html>
END

module Bonjour
  module_function
  def publish(name = SERVER_NAME, port = SERVER_PORT)
    netservice = NSNetService.alloc.initWithDomain("", type:"_http._tcp", name:name, port:port)
    netservice.publish()
  end
end

class SystemControl
  def initialize
    Bonjour.publish(SERVER_NAME, SERVER_PORT)
    @volume = System::Sound.volume
  end

  def call(env)
    req = Rack::Request.new(env)
    
    case req.path_info
    when "/sleep"
      System::Power.sleep
    when "/vol_up"
      @volume += 0.2
      @volume  = 1.0 if(@volume > 1.0)
      System::Sound.set_volume(@volume)
    when "/vol_down"
      @volume -= 0.2
      @volume  = 0.0 if(@volume < 0.0)
      System::Sound.set_volume(@volume)
    when "/app_itunes"
      system "open -a 'iTunes'"
    end

    [200, { 'Content-Type' => 'text/html; charset=UTF-8' }, HTML]
  end

end

run SystemControl.new
