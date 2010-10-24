require 'test/unit'
require 'system_control'

class TestNetwork < Test::Unit::TestCase

  def test_wake_on_lan
    assert_raise(ArgumentError){ System::Network.wake        }
    assert_raise(TypeError)    { System::Network.wake(1)     }
    assert_raise(ArgumentError){ System::Network.wake "1:2:3:4:5:6:7" }
    assert_raise(ArgumentError){ System::Network.wake "1:2:3:" }
    assert_raise(ArgumentError){ System::Network.wake "111:2:3:4:5:6" }
    assert_raise(ArgumentError){ System::Network.wake "1:cg:3:4:5:6" }
    assert_nothing_raised(ArgumentError){ System::Network.wake "1:2:3:4::6" }
  end

end
