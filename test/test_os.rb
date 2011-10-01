framework 'Cocoa'
require 'test/unit'
require 'system_control'

class TestSound < Test::Unit::TestCase

  def test_version
    version = System::OS::version

    ver = NSProcessInfo.processInfo.operatingSystemVersionString
    ver =~ /Version (\d+)\.(\d+).(\d+) /
    ver = "#{$1}#{$2}#{$3}"
    assert_equal(ver, sprintf("%x", version))
  end

end
