<?php
// $Id$

function driven_props_overview_form($form, &$form_state, $driver_mod, $node_type) {
  $form = array(
    '#node_type' => $node_type->type,
    '#driver_mod' => $driver_mod,
  );
  
  $hook = 'driven_props_type_settings';
  $type_settings = module_invoke($driver_mod, $hook);

  $args_what = array('@what' => $type_settings['description_tail']);
  
  $form['driven_props'] = array(
    '#type' => 'fieldset',
    '#title' => t('Driven properties'),
    '#description' => t('Select which of the available properties will be <strong>@what</strong>.', $args_what),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  
  $form['driven_props']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable driven properties'),
    '#description' => t('Disabling this option would be enough to make all properties bellow ignored.'),
    '#default_value' => FALSE,
  );
  
  $available_props = driven_properties_available($node_type->type);
  $unsupported_props = array();
  $hook = 'unsupported_driven_props';
  if (module_hook($driver_mod, $hook)) {
    $unsupported_props = module_invoke($driver_mod, $hook, $available_props);
  }
  
  if (empty($available_props)) {
    $form['driven_props']['no_props'] = array(
      '#type' => 'item',
      '#markup' => t('There are no driven properties exposed. You have to enable some property provider modules.'),
      '#prefix' => '<div class="warning messages">',
      '#suffix' => '</div>',
    );
  }
  foreach ($available_props as $property_id => $prop) {
    $realm = $prop['realm'];
    $target = $prop['target'];
    $elem = &$form['driven_props'][$realm];
    if (!isset($elem)) {
      $elem = array(
        '#type' => 'fieldset',
        '#title' => $prop['realm_title'],
        '#collapsible' => TRUE,
      );
    }
    $elem[$target]['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => $prop['target_title'],
      '#default_value' => FALSE,
    );
    if (isset($unsupported_props[$property_id])) {
      $elem[$target]['enabled']['#disabled'] = TRUE;
      $elem[$target]['enabled']['#description'] = $unsupported_props[$property_id];
    }
    else {
      $elem[$target]['behaviors'] = _driven_props_retrieve_behaviors_form($driver_mod, $prop);
    }
  }
  $varname = $driver_mod . ':type:' . $node_type->type . ':driven_props';
  $settings = variable_get($varname, array());
  _driven_props_load_settings($form['driven_props'], $settings);
  
  $hook = 'driven_props_type_settings_form';
  if (module_hook($driver_mod, $hook)) {
    $form['driver_mod'] = module_invoke($driver_mod, $hook, $node_type->type);
    $form['driver_mod']['#tree'] = TRUE;
    $varname = $driver_mod . ':type:' . $node_type->type . ':settings';
    $settings = variable_get($varname, array());
    _driven_props_load_settings($form['driver_mod'], $settings);
  }
  
  $form += driven_form_signature();
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['buttons']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
  );
  $form['#theme'] = 'system_settings_form';
  
  return $form;
}

function _driven_props_load_settings(&$form, $settings) {
  foreach (element_children($form) as $key) {
    if (!isset($settings[$key])) {
      // stored settings are #tree
      // if there is nothing at this level then this is a dead end
      continue; 
    } 
    if (array_key_exists('#default_value', $form[$key])) {
      // replace the default if a stored value exists
      $form[$key]['#default_value'] = $settings[$key];
    }
    else {
      _driven_props_load_settings($form[$key], $settings[$key]);
    }
  }
}

function driven_props_overview_form_submit($form, $form_state) {
  $node_type = $form['#node_type'];
  $driver_mod = $form['#driver_mod'];
  $values = $form_state['values'];
  $op = isset($values['op']) ? $values['op'] : '';
  
  if ($op == t('Reset to defaults')) {
    driven_props_delete_settings($driver_mod, $node_type);
  }
  else {
    // IMPORTANT: modules are responsible of
    // deleting their variables on uninstall
  
    // at least we have the 'enabled' key
    $varname = $driver_mod . ':type:' . $node_type . ':driven_props';
    variable_set($varname, $values['driven_props']);

    $varname = $driver_mod . ':type:' . $node_type . ':settings';
    if (isset($values['driver_mod'])) {
      variable_set($varname, $values['driver_mod']);
    }
    else {
      // if the module is not exposing settings
      // avoid old garbage settings from being picked up later
      variable_del($varname);
    }
  }
}
