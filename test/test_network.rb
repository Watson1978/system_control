require 'test/unit'
require 'system_control'

class TestNetwork < Test::Unit::TestCase

  def test_wake_on_lan
    assert_raise(ArgumentError){ System::Network.wol        }
    assert_raise(TypeError)    { System::Network.wol(1)     }
    assert_raise(ArgumentError){ System::Network.wol "1:2:3:4:5:6:7" }
    assert_raise(ArgumentError){ System::Network.wol "111:2:3:4:5:6" }
    assert_raise(ArgumentError){ System::Network.wol "1:cg:3:4:5:6" }
    assert_raise(ArgumentError){ System::Network.wol "1:2:3:4::6" }
  end

end
