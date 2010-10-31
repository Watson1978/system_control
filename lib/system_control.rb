require 'system_control/system_control'

module System
  module Pboard
    framework 'AppKit'

    module_function

    # call-seq: System::Pboard.paste
    #
    # This method get string from paste board.

    def paste
      pboard = NSPasteboard.generalPasteboard
      pboard.stringForType(NSStringPboardType)
    end

    # call-seq: System::Pboard.copy(string) -> true or false
    #
    # This method set string to paste board.

    def copy(string)
      if (string.class != String)
        string = string.to_s
      end

      pboard = NSPasteboard.generalPasteboard
      pboard.declareTypes([NSStringPboardType], owner:nil);
      pboard.setString(string, forType:NSStringPboardType)
    end
  end
end

