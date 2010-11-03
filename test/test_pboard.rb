require 'test/unit'
require 'system_control'

class TestPboard < Test::Unit::TestCase
  def test_pboard
    System::Pboard.copy 'hello world'
    assert_equal('hello world', System::Pboard.paste)

    System::Pboard.clear
    assert_equal('', System::Pboard.paste)
  end

  def test_pboard_read_write
    System::Pboard.write 'hello world'
    assert_equal('hello world', System::Pboard.read)
  end

end
