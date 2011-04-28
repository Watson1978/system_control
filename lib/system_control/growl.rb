module System
  class Growl
    framework 'Cocoa'
    framework 'Foundation'
    require 'singleton'
    include Singleton

    GROWL_IS_READY = "Lend Me Some Sugar; I Am Your Neighbor!"
    GROWL_NOTIFICATION_CLICKED = "GrowlClicked!"
    GROWL_NOTIFICATION_TIMED_OUT = "GrowlTimedOut!"
    GROWL_KEY_CLICKED_CONTEXT = "ClickedContext"

    # Regists a application metadata for Growl notification.
    #
    # @param [String] app
    #   The application name that should be used registration in Growl.
    # @param [Array] notifications
    #   The kind of notifications.
    # @param [NSImage] icon
    #   The application icon that should be used registration/notification in Growl.
    def regist(app, notifications, icon = nil)
      @application_name = app.to_s
      @application_icon = icon || NSApplication.sharedApplication.applicationIconImage
      @notifications = notifications.map!{|x| x.to_s}
      @default_notifications = notifications
      @center = NSDistributedNotificationCenter.defaultCenter
      send_registration!
    end

    # Sends a Growl notification.
    #
    # @param [String] notification
    #   The one of notifications that registed notifications with regist.
    # @param [String] title
    #   The title that should be used in the Growl notification.
    # @param [String] description
    #   The body of the Grow notification.
    # @param [Hash] options
    #   Specifies a few optional options.<br>
    #   * :sticky - indicates if the Grow notification should "stick" to the screen. Defaults to +false+.<br>
    #   * :priority - sets the priority level of the Growl notification. Defaults to 0.
    def notify(notification, title, description, options = {})
      dict = {
        :ApplicationName => @application_name,
        #:ApplicationPID => pid,
        :NotificationName => notification.to_s,
        :NotificationTitle => title.to_s,
        :NotificationDescription => description.to_s,
        :NotificationPriority => options[:priority] || 0,
        :NotificationIcon => @application_icon.TIFFRepresentation,
      }
      dict[:NotificationSticky] = 1 if options[:sticky]

      @center.postNotificationName(:GrowlNotification, object:nil, userInfo:dict, deliverImmediately:false)
    end

    private
    def send_registration!
      dict = {
        :ApplicationName => @application_name,
        :ApplicationIcon => @application_icon.TIFFRepresentation,
        :AllNotifications => @notifications,
        :DefaultNotifications => @default_notifications
      }

      @center.postNotificationName(:GrowlApplicationRegistrationNotification, object:nil, userInfo:dict, deliverImmediately:true)
    end
  end
end

module Kernel
  unless(defined? g)

    #
    # Sends a Growl Notification. This method needs three arguments(two arguments are optional).
    #
    # @param [String] title
    #   The title that should be used in the Growl notification. This is optional argument. Defaults to "MacRuby".
    # @param [String] description
    #   The body of the Grow notification.
    # @param [Hash] options
    #   Specifies a few optional options.<br>
    #   * :sticky - indicates if the Grow notification should "stick" to the screen. Defaults to +false+.<br>
    #   * :priority - sets the priority level of the Growl notification. Defaults to 0.
    #
    # @example
    #   g "Description"
    #   g "Description", {:sticky => true}
    #   g "Notification title", "Description"
    #   g "Notification title", "Description", {:sticky => true}
    def g(*args)
      size = args.size
      raise ArgumentError, "wrong number of arguments" if size <= 0 || size > 3

      case size
      when 1
        title, description = "MacRuby", args[0]
      when 2, 3
        title, description, opts = args[0 .. 2]
        if description.kind_of?(Hash)
          title, description, opts = "MacRuby", title, description
        end
      end

      opts = {} if opts.nil?
      raise ArgumentError, "illigal arguments" unless opts.kind_of?(Hash)

      g = System::Growl.instance
      icon = g.instance_variable_get(:@application_icon)

      if icon.nil?
        icon_path = File.expand_path(File.join(__FILE__, '../../../resources/macruby.icns'))
        icon = NSImage.alloc.initWithContentsOfFile(icon_path)
      end

      notification = 'notification'
      g.regist("MacRuby System Control", [notification], icon)
      g.notify(notification, title, description, opts)
    end
  end
end
