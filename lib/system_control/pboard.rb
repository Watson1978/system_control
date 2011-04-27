module System
  module Pboard
    framework 'AppKit'

    module_function

    # call-seq:
    #   System::Pboard.paste -> string
    #
    # This method gets a string from the pasteboard.
    def paste
      pboard = NSPasteboard.generalPasteboard
      str   = pboard.stringForType(NSStringPboardType)
      str ||= ""
    end

    # call-seq:
    #    System::Pboard.read -> string
    #
    # This method gets a string from the pasteboard.
    # - Alias of System::Pboard.paste
    def read
      self.paste
    end

    # call-seq:
    #   System::Pboard.copy(string)
    #
    # This method sets a string to the pasteboard.
    def copy(string)
      if (string.class != String)
        string = string.to_s
      end

      pboard = NSPasteboard.generalPasteboard
      pboard.declareTypes([NSStringPboardType], owner:nil);
      pboard.setString(string, forType:NSStringPboardType)
    end

    # call-seq:
    #   System::Pboard.write(string)
    #
    # This method sets a string to the pasteboard.
    # - Alias of System::Pboard.copy
    def write(string)
      self.copy(string)
    end

    # call-seq:
    #   System::Pboard.clear
    #
    # Clears the existing contents of the pasteboard.
    def clear
      pboard = NSPasteboard.generalPasteboard
      pboard.clearContents
      nil
    end
  end
end
