<?php

function theme_selectandprint_list(&$vars) {
  $output = '';

  foreach ($vars['data']['group'] as $key => $group) {
    if (sizeof($vars['data']['selectandprint_item'][$key]) > 0) {
      $output .= '<h3>' . $group . '</h3>';
      $items = array();
      foreach ($vars['data']['selectandprint_item'][$key] as $item) {
        $items[] = $item;
      }
      $output .= theme('item_list', array('items' => $items));
    }
  }

  if ($vars['show_credits']) {
    $credits = variable_get('selectandprint_credits', '');
    if (!empty($credits)) {
      $output .= '<br /><hr />';
      $output .= '<div class="credits">' . $credits . '</div>';
    }
  }

  return $output;
}