module System
  module Pboard
    framework 'AppKit'

    module_function

    # This method gets a string from the pasteboard.
    #
    # @return [String]
    #   The string that gets from the pasteboard.
    def paste
      pboard = NSPasteboard.generalPasteboard
      str   = pboard.stringForType(NSStringPboardType)
      str ||= ""
    end

    # This method gets a string from the pasteboard. This method is alias of System::Pboard.paste.
    #
    # @return [String]
    #   The string that gets from the pasteboard.
    def read
      self.paste
    end

    # This method sets a string to the pasteboard.
    #
    # @param [String] string
    #   The string that sets to the pasteboard.
    def copy(string)
      pboard = NSPasteboard.generalPasteboard
      pboard.declareTypes([NSStringPboardType], owner:nil);
      pboard.setString(string.to_s, forType:NSStringPboardType)
    end

    # This method sets a string to the pasteboard. This method is alias of System::Pboard.copy
    #
    # @param [String] string
    #   The string that sets to the pasteboard.
    def write(string)
      self.copy(string)
    end

    # Clears the existing contents of the pasteboard.
    def clear
      pboard = NSPasteboard.generalPasteboard
      pboard.clearContents
      nil
    end
  end
end
