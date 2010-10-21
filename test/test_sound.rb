require 'test/unit'
require 'system_control'

class TestSound < Test::Unit::TestCase
  def cut(n)
    n = (n * 100).floor.to_f
    n / 100.0
  end

  def test_volume
    System::Sound.set_volume(0.25)
    assert_equal(0.25, cut(System::Sound.volume))

    System::Sound.volume = 0.30
    assert_equal(0.30, cut(System::Sound.volume))

    System::Sound.set_volume(1.0)
    assert_equal(1.0, cut(System::Sound.volume))

    System::Sound.set_volume(0.0)
    assert_equal(0.0, cut(System::Sound.volume))

    assert_raise(TypeError)    { System::Sound.set_volume(1)     }
    assert_raise(ArgumentError){ System::Sound.set_volume        }
    assert_raise(RangeError)   { System::Sound.set_volume(-0.01) }
    assert_raise(RangeError)   { System::Sound.set_volume(1.01)  }
  end

end
