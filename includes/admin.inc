<?php

function selectandprint_admin_settings_form() {
  $fid = variable_get('selectandprint_header_fid', '');
  if ($fid) {
    $file = file_load($fid);
    if ($file) {
      $image = theme('image_style', array('style_name' => 'medium', 'path' => $file->uri));
      $form['selectandprint_header_existing'] = array(
          '#type' => 'item',
          '#title' => t('Existing letterhead'),
          '#markup' => $image,
      );
      $header_exists = TRUE;
    }
  }
  $form['selectandprint_header'] = array(
      '#type' => 'file',
      '#title' => ($header_exists) ? t('Upload a new letterhead') : t('Letterhead image'),
      '#description' => t('This will show up as a "letterhead" at the top of printable lists. This field is optional.'),
  );

  $form['selectandprint_credits'] = array(
      '#type' => 'textarea',
      '#title' => t('Credits'),
      '#description' => t('Credits show up at the bottom of printable lists. This field is optional.'),
      '#default_value' => variable_get('selectandprint_credits', ''),
  );

  $form['#submit'][] = 'selectandprint_admin_settings_form_submit';

  return system_settings_form($form);
}

function selectandprint_admin_settings_form_submit(&$form, &$form_state) {
  $validators = array(
      'file_validate_is_image' => array(),
  );

  $file = file_save_upload('selectandprint_header', $validators);
  if ($file !== NULL) {

    $file_directory = variable_get('file_default_scheme', 'public') . '://';
    $destination = file_stream_wrapper_uri_normalize($file_directory . '/' . $file->filename);
    $file = file_move($file, $destination, FILE_EXISTS_RENAME);
    if ($file) {
      $file->status |= FILE_STATUS_PERMANENT;
      file_save($file);

      variable_set('selectandprint_header_fid', $file->fid);
    }
  }
  else {
    variable_del('selectandprint_header_fid');
  }
}