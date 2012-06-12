(function ($) {
  Drupal.behaviors.SelectAndPrint = {
    attach : function(context) {
      var sap = $('div.selectandprint-section');
      
      // All hyperlinks inside of a select and print list should open
      // in a new window
      $('a', sap).attr('target', '_blank');
      
      var printClick = function() {
        var title = $('h1').first().text();
        var wrapper = $(this).parents('div.selectandprint-section');
        var form = $('form', wrapper);
        var iframe = $('#' + form.attr('target'));
        var groups = [];
        
        if (title.length > 0) {
          $('input.title', wrapper).val(title);
        }
        
        form.trigger('submit');
        iframe.bind('load', function(){
          if (navigator.appName != 'Microsoft Internet Explorer'){
            window.frames[iframe.attr('name')].print();
          }else{ 
            document[iframe.attr('name')].focus();
            document[iframe.attr('name')].print();
          }
        });
      };
      
      var selectAll = function() {
        var wrapper = $(this).parents('div.selectandprint-section');
        var form = $('form', wrapper);
        $('input:checkbox', form).attr('checked', this.checked);
      };  
      
      $('.selectandprint-button', sap).click(printClick);
      $('.selectandprint-selectall', sap).click(selectAll);
      
      
    }
  };
})(jQuery);