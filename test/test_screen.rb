require 'test/unit'
require 'system_control'

class TestScreen < Test::Unit::TestCase
  def cut(n)
    n = (n * 100).floor.to_f
    n / 100.0
  end

  def test_brightness
    System::Screen.brightness = 0.25
    assert_equal(0.25, cut(System::Screen.brightness))

    assert_raise(TypeError)    { System::Screen.brightness = 1     }
    assert_raise(RangeError)   { System::Screen.brightness = -0.01 }
    assert_raise(RangeError)   { System::Screen.brightness = 1.01  }
  end

end
