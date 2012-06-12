<?php

/**
 * Implementation of hook_permission().
 */
function selectandprint_permission() {
  $items = array();

  $items['administer select and print settings'] = array(
      'title' => t('Administer Select and Print settings'),
      'description' => t('Allows access to the Select and Print admin settings form'),
  );

  return $items;
}

/**
 * Implementation of hook_init().
 */
function selectandprint_init() {
  $path = drupal_get_path('module', 'selectandprint');

  drupal_add_js("$path/js/selectandprint.js");

  drupal_add_css("$path/css/selectandprint.css");
  drupal_add_css("$path/css/print.css", array('media' => 'print'));
}

/**
 * Implementation of hook_menu().
 */
function selectandprint_menu() {
  $items = array();

  $items['admin/config/selectandprint'] = array(
      'title' => 'Select and Print',
      'position' => 'right',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('administer site configuration'),
      'file' => 'system.admin.inc',
      'file path' => drupal_get_path('module', 'system'),
  );

  $items['admin/config/selectandprint/settings'] = array(
      'title' => 'Select and Print Settings',
      'description' => 'Configuration options for the Select and Print module.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('selectandprint_admin_settings_form'),
      'access arguments' => array('administer select and print settings'),
      'file' => 'includes/admin.inc',
  );

  $items['selectandprint/%ctools_js/print'] = array(
      'title callback' => 'selectandprint_page_print_title',
      'page callback' => 'selectandprint_page_print',
      'page arguments' => array(1),
      'access callback' => TRUE,
      'type' => MENU_CALLBACK,
      'file' => 'includes/pages.inc',
  );
  $items['selectandprint/print'] = array(
      'page callback' => 'selectandprint_iframe_print',
      'access callback' => TRUE,
      'type' => MENU_CALLBACK,
      'file' => 'includes/pages.inc',
  );

  return $items;
}

/**
 * Implementation of hook_theme().
 */
function selectandprint_theme() {
  $items = array();

  $items['selectandprint_list'] = array(
      'variables' => array(
          'data' => array(),
          'show_credits' => TRUE,
      ),
      'file' => 'includes/theme.inc',
  );

  return $items;
}

/**
 * Implementation of hook_filter_info().
 */
function selectandprint_filter_info() {
  $filters = array();
  $filters['selectandprint'] = array(
      'title' => t('Select and Print'),
      'description' => t('Converts text to a selectable, printable list of items (ex. grocery list)'),
      'process callback' => 'selectandprint_filter_process',
      'tips callback' => 'selectandprint_filter_tips',
  );
  return $filters;
}

/**
 * Select and Print filter process callback.
 * 
 * The actual filtering is performed here. The supplied text should be
 * returned, once any necessary substitutions have taken place.
 */
function selectandprint_filter_process($text, $filter, $format) {
  while ($start = stripos($text, '[selectandprint]')) {
    $end = stripos($text, '[/selectandprint]');

    if ($end) {
      $end += 17; // add length of [/selectandprint] closing tag
    }
    else {
      // improperly formatted
      break;
    }

    // Pull contents of string between $start and $end into a new variable
    $section = substr($text, $start, $end - $start);

    // Standardize line endings
    $section = str_replace("\r\n", "\n", $section);
    //$section = str_replace("\n", "", $section);
    // Strip out [selectandprint] [/selectandprint] tags
    $section = preg_replace('/\[\/?selectandprint\]/', '', $section);
    $section = str_replace("<br />", "\n", $section);
    $section = filter_xss($section, array('a', 'em', 'strong', 'b', 'i', 'u'));

    //dpm($section);
    // Split substring into an array of lines
    $lines = explode("\n", $section);
    $section = '';
    $i = 0;
    $g = 0;
    $tables = 0;
    $cb_template = '<td class="checkbox"><input type="checkbox" class="checkbox" id="selectandprint-item-%d-%d" name="selectandprint_item[%d][%d]" value="%s" /></td><td class="label"><label for="selectandprint-item-%d-%d">%s</label></td>';

    $guid = uniqid();
    $section .= '<div class="selectandprint-section">';
    $section .= '<div class="selectandprint-section-inner">';
    $section .= sprintf('<form action="%s" method="post" target="%s" class="selectandprint-form">', url('selectandprint/print'), $guid);
    $section .= '<input type="hidden" name="title" class="title" value="" />';
    $section .= '<input type="hidden" name="groups" class="groups" value="" />';
    $section .= sprintf('<input type="hidden" name="guid" class="guid" value="%s" />', $guid);
    $section .= sprintf('<IFRAME name="%s" id="%s" width="1" height="1" frameborder="0"></IFRAME>', $guid, $guid);


    $section .= '<div class="selectandprint-selectall-wrapper">';
    $section .= sprintf('<input type="checkbox" name="selectall" class="selectandprint-selectall" id="%s-selectall" />', $guid);
    $section .= sprintf(' <label for="%s-selectall">%s</label>', $guid, t('Select All Items'));
    $section .= '</div>';

    // Loop through and translate the selectandprint text into HTML
    for ($l = 0; $l < sizeof($lines); $l++) {
      $line = $lines[$l];

      $line = str_replace('/^\s*(.+)\s*$/', "\1", $line);

      // A version of the line with NO html, used for checking and rendering group headers
      $line_stripped = trim(filter_xss($line, array()));

      // Convert *header* lines into <h3></h3> tags
      if ($line_stripped[0] == '@') {
        if ($tables++ > 0) {
          ++$g; // increment group counter
          $section .= '</table>';
          $section .= '</div>';
          $unclosed = FALSE;
        }
        $section .= '<div class="selectandprint-group-wrapper">';
        $section .= '<h3 class="selectandprint-group">' . substr($line_stripped, 1) . '</h3>';
        $section .= sprintf('<input type="hidden" name="group[%s]" value="%s" />', $g, substr($line_stripped, 1));

        if ($l < sizeof($lines) - 1) {
          $section .= '<table class="selectandprint-table">';
          $unclosed = TRUE;
        }
      }
      else if (strlen(trim($line)) > 0) {
        $section .= '<tr class="selectandprint-item">';
        $section .= sprintf($cb_template, $g, $i, $g, $i, $line_stripped, $g, $i, $line);
        $section .= '</tr>';
        $i++;
      }
    }
    if ($unclosed) {
      $section .= '</table>';
      $section .= '</div>';
    }

    if ($i > 0) {
      $section .= '<div class="selectandprint-selectall-wrapper">';
      $section .= sprintf('<input type="checkbox" name="selectall" class="selectandprint-selectall" id="%s-selectall" />', $guid);
      $section .= sprintf(' <label for="%s-selectall">%s</label>', $guid, t('Select All Items'));
      $section .= '</div>';

      $section .= sprintf('<input type="button" class="selectandprint-button" value="%s"></button>', t('Print this List'));
    }
    $section .= '</form>';
    $section .= '</div>';
    $section .= '</div>';

    // Save changes back into the original text string
    $text = substr_replace($text, $section, $start, $end - $start);
  }

  return $text;
}

/*
 * Select and Print filter tips callback.
 */

function selectandprint_filter_tips($filter, $format, $long = FALSE) {
  return t('Begin your select and print list with [selectandprint]. End it with [/selectandprint]. Put each list item on its own line. Separate your list items into groups by adding a @ at the beginning of the line where you want a group header');
}