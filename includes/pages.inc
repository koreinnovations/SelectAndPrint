<?php

/**
 * Renders a printable list from a selectandprint form post
 */
function selectandprint_page_print($js = FALSE, $_cache_key = '') {
  // Disable caching for this page
  $GLOBALS['conf']['cache'] = FALSE;

  ctools_include('modal');
  ctools_include('ajax');

  // Store the post in the session so that we can render it in our IFRAME also
  if (empty($_cache_key)) {
    $_cache_key = uniqid();
    $_SESSION[$_cache_key] = $_POST;

    $url = sprintf('selectandprint/%s/print/%s', ($js) ? 'ajax' : 'nojs', $_cache_key);

    drupal_goto($url);
  }

  $output = theme('selectandprint_list', array('data' => $_SESSION[$_cache_key]));
  $output .= sprintf('<IFRAME src="%s" width="1" height="1" frameborder="0" style="display:none"></IFRAME>', url('selectandprint/print/' . $_cache_key));

  if ($js) {
    ctools_modal_render(selectandprint_page_print_title($_SESSION[$_cache_key]), $output);
  }
  else {
    drupal_set_title(selectandprint_page_print_title($_SESSION[$_cache_key]));
    return $output;
  }
}

/**
 * Title callback for printable list page
 */
function selectandprint_page_print_title($data = array()) {
  if (empty($data)) {
    $data = $_POST;
  }
  return ($data['title']) ? $data['title'] : t('Printable List');
}

/**
 * Page callback function for plain and simple note printing.
 */
function selectandprint_iframe_print($_cache_key = '') {
  $path = drupal_get_path('module', 'selectandprint');
  
  if (empty($_cache_key)) {
    $_cache_key = uniqid();
    $_SESSION[$_cache_key] = $_POST;

    $url = sprintf('selectandprint/%s/print/%s', ($js) ? 'ajax' : 'nojs', $_cache_key);

    drupal_goto($url);
  }
  

  $title = selectandprint_page_print_title($_SESSION[$_cache_key]);

  $output = '';

  $output .= '<html>';
  $output .= '<head>';
  $output .= '<title>' . $title . '</title>';
  $output .= '<script src="/misc/jquery.js" type="text/javascript"></script>';
  $output .= '<script type="text/javascript" src="/' . $path . '/js/selectandprint_iframe.js"></script>';
  $output .= '</head>';
  $output .= '<body class="selectandprint-page">';

  $header_fid = variable_get('selectandprint_header_fid', '');
  if ($header_fid) {
    $file = file_load($header_fid);
    $output .= theme('image', array('path' => $file->uri));
  }

  $output .= '<h1>' . $title . '</h1>';

  $output .= theme('selectandprint_list', array('data' => $_SESSION[$_cache_key], 'show_header' => TRUE, 'show_credits' => TRUE));

  $output .= '</body>';
  $output .= '</html>';

  print $output;

  exit;
}