require 'system_control/system_control'

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

  module Power
    module_function

    # call-seq:
    #   System::Power.no_sleep {
    #      # Add the work you need to do without
    #      # the system sleeping here.
    #   }
    #
    # While this method processes given block, the system does not sleep.
    def no_sleep(&block)
      if !block_given?
        raise ArgumentError, "need block"
      end

      begin
        id = no_sleep_open()
        block.call
      ensure
        no_sleep_close(id)
      end
    end
  end
end

