framework 'Cocoa'
require 'test/unit'
require 'system_control'

class TestPower < Test::Unit::TestCase

  def test_battery
    battery = System::Power.battery_info
    p battery

    if System::Power.is_AC?
      assert_equal(0, battery["Time to Empty"])
    end
  end

end
