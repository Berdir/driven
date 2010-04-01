<?php
// $Id$

// helper functions for property provider modules

function driven_diff_values_non_array($element, $form_values) {
  $default = driven_elem_default_value($element);
  // $value will be normized if needed (per case basis)
  // to match #default_value format instead of $form_state['values']
  $value = driven_elem_state_value($element, $form_values);
  
  if (_driven_equal_non_array($default, $value)) {
    return FALSE;
  }
  return array($default, $value);
}

function driven_diff_values_keyed_array_in_depth($element, $form_values) {
  $default = driven_elem_default_value($element);
  // $value will be normized if needed (per case basis)
  // to match #default_value format instead of $form_state['values']
  $value = driven_elem_state_value($element, $form_values);
  
  if (driven_equal_keyed_array_in_depth($default, $value)) {
    return FALSE;
  }
  return array($default, $value);
}

function driven_diff_output_non_array($outgoing, $incoming, $theme = '', $theme_varname = 'value') {
  $outgoing = driven_diff_output($theme, array($theme_varname => $outgoing));
  $incoming = driven_diff_output($theme, array($theme_varname => $incoming));
  return array($outgoing, $incoming);
}

// WARNING: use with care
// there are weird cases where this would be a proper normalization
// and ALWAYS AFTER normalizing the relevant keys that shouldn't be dropped from $value
// (e.g. fieldfield's upload related data, date's second value when only one is in use)
function driven_diff_limit_value_keys($default, $value) {
  // remove from $value keys which are not present in $default
  $keys = array_diff(array_keys($value), array_keys($default));
  foreach ($keys as $key) {
    unset($value[$key]);
  }
  return $value;
}
