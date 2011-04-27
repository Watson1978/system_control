require File.join(File.dirname(__FILE__), 'system_control')

module System
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
