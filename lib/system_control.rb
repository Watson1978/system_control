require 'system_control/system_control'

module System
  module Pboard
    framework 'AppKit'

    module_function

    # call-seq: System::Pboard.paste -> string
    #
    # This method get string from tbe pasteboard.

    def paste
      pboard = NSPasteboard.generalPasteboard
      str   = pboard.stringForType(NSStringPboardType)
      str ||= ""
    end

    # call-seq: System::Pboard.copy(string) -> true or false
    #
    # This method set string to the pasteboard.

    def copy(string)
      if (string.class != String)
        string = string.to_s
      end

      pboard = NSPasteboard.generalPasteboard
      pboard.declareTypes([NSStringPboardType], owner:nil);
      pboard.setString(string, forType:NSStringPboardType)
    end

    # call-seq: System::Pboard.clear
    #
    # Clears the existing contents of the pasteboard.

    def clear
      pboard = NSPasteboard.generalPasteboard
      pboard.clearContents
      nil
    end
  end
end

