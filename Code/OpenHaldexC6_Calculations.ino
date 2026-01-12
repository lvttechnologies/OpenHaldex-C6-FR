// Seulement exécuté lorsqu'en MODE_FWD/MODE_5050/MODE_CUSTOM
float get_lock_target_adjustment() {
  // Gestion des modes FWD et 5050.
  switch (state.mode) {
    case MODE_FWD:
      return 0;

    case MODE_5050:
      if (state.pedal_threshold == 0 && disableSpeed == 0) {
        return 100;
      }

      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 100;
      }
      return 0;

    case MODE_6040:
      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 40;
      }
      return 0;

    case MODE_7525:
      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 30;
      }
      return 0;

    default:
      return 0;
  }

  // Se rendre ici signifie qu'il n'est pas FWD ou 5050/7525.

  // Vérifier si le verrouillage est nécessaire.
  if (!(int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override)) {
    return 0;
  }

  // Trouver la paire de points de verrouillage entre laquelle la vitesse du véhicule chute.
  lockpoint_t lp_lower = state.custom_mode.lockpoints[0];
  lockpoint_t lp_upper = state.custom_mode.lockpoints[state.custom_mode.lockpoint_count - 1];

  // Déterminer le point de verrouillage au-dessus de la vitesse actuelle.
  for (uint8_t i = 0; i < state.custom_mode.lockpoint_count; i++) {
    if (received_vehicle_speed <= state.custom_mode.lockpoints[i].speed) {
      lp_upper = state.custom_mode.lockpoints[i];
      lp_lower = state.custom_mode.lockpoints[(i == 0) ? 0 : (i - 1)];
      break;
    }
  }

  // Gestion du cas ou la vitesse du véhicule est inférieure au point de verrouillage le plus bas.
  if (received_vehicle_speed <= lp_lower.speed) {
    return lp_lower.lock;
  }

  // Gestion du cas ou la vitesse du véhicule est supérieure au point de verrouillage le plus haut.
  if (received_vehicle_speed >= lp_upper.speed) {
    return lp_upper.lock;
  }

  // Dans tous les autres cas, l'interpolation est nécessaire..
  float inter = (float)(lp_upper.speed - lp_lower.speed) / (float)(received_vehicle_speed - lp_lower.speed);

  // Calculer la cible.
  float target = lp_lower.lock + ((float)(lp_upper.lock - lp_lower.lock) / inter);
  //DEBUG("lp_upper:%d@%d lp_lower:%d@%d speed:%d target:%0.2f", lp_upper.lock, lp_upper.speed, lp_lower.lock, lp_lower.speed, received_vehicle_speed, target);
  return target;
}

// Seulement exécuté lorsqu'en MODE_FWD/MODE_5050/MODE_CUSTOM
uint8_t get_lock_target_adjusted_value(uint8_t value, bool invert) {
  // Gestion du mode 5050.
  if (lock_target == 100) {
    // est-ce nécessaire?  Devrait être saisi dans get_lock_target_adjustment
    if (int(received_pedal_value) >= state.pedal_threshold || received_vehicle_speed < disableSpeed || state.pedal_threshold == 0) {
      return (invert ? (0xFE - value) : value);
    }
    return (invert ? 0xFE : 0x00);
  }

  // Gestion des modes FWD et CUSTOM.
  // Aucune correction nécessaire si la cible est 0.
  if (lock_target == 0) {
    return (invert ? 0xFE : 0x00);
  }

  float correction_factor = ((float)lock_target / 2) + 20;
  uint8_t corrected_value = value * (correction_factor / 100);
  if (int(received_pedal_value) >= state.pedal_threshold || received_vehicle_speed < disableSpeed || state.pedal_threshold == 0) {
    return (invert ? (0xFE - corrected_value) : corrected_value);
  }
  return (invert ? 0xFE : 0x00);
}

// Seulement exécuté lorsqu'en MODE_FWD/MODE_5050/MODE_CUSTOM
void getLockData(twai_message_t& rx_message_chs) {
  // Obternir la cible de verrouillage initiale.
  lock_target = get_lock_target_adjustment();

  // Modifier les trames si configuré en Gen1...
  if (haldexGeneration == 1) {
    switch (rx_message_chs.identifier) {
      case MOTOR1_ID:
        rx_message_chs.data[0] = 0x00;                                         // octets individuels variés ('space gas', driving pedal, kick down, clutch, timeout brake, brake intervention, "drinks-torque intervention"?) était 0x01 - ignoré
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);  // rpm low byte
        rx_message_chs.data[2] = 0x21;                                         // rpm high byte
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);  // régler les tours-minute à une valeur pour que la pompe de précharge marche
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // inner moment (%): 0.39*(0xF0) = 93.6%  (make FE?) - ignoré
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // driving pedal (%): 0.39*(0xF0) = 93.6%  (make FE?) - ignoré
                                                                               // rx_message_chs.data[6] = get_lock_target_adjusted_value(0x16, false);  // régler à une valeur basse poru contrôler le transfer torque requis.  Valeur de contrôle principale pour Gen1
        switch (state.mode) {
          case MODE_FWD:
            appliedTorque = get_lock_target_adjusted_value(0xFE, true);  // retourner 0xFE pour désactiver
            break;
          case MODE_5050:
            appliedTorque = get_lock_target_adjusted_value(0x16, false);  // retourner 0x16 pour verouiller pleinement
            break;
          case MODE_6040:
            appliedTorque = get_lock_target_adjusted_value(0x22, false);  // régler à ~30% de verrouillage (0x96 = 15%, 0x56 = 27%)
            break;
          case MODE_7525:
            appliedTorque = get_lock_target_adjusted_value(0x50, false);  // régler à ~30% verrouillage (0x96 = 15%, 0x56 = 27%)
            break;
        }

        rx_message_chs.data[6] = appliedTorque;  // était 0x00
        rx_message_chs.data[7] = 0x00;           // celles-ci doivent jouer un rôle - atteint ~169 sans
        break;
      case MOTOR3_ID:
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // pédale - ignoré
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0xFE, false);  // throttle angle (100%), ignoré
        break;
      case BRAKES1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0x00, false);  // contrôle aussi le patinage.  La force de freinage peut ajouter 20%
        rx_message_chs.data[2] = 0x00;                                         //  ignoré
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x0A, false);  // 0xA ignorée?
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);  // low byte, Av. GAUCHE // affecte légèrement +2
        rx_message_chs.data[1] = 0x0A;                                         // high byte, Av. GAUCHE grand effet
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // low byte, Av. DROITE // affecte légèrement +2
        rx_message_chs.data[3] = 0x0A;                                         // high byte, Av. DROITE grand effet
        rx_message_chs.data[4] = 0x00;                                         // low byte, Arr. GAUCHE
        rx_message_chs.data[5] = 0x0A;                                         // high byte, Arr. GAUCHE // 254+10? (5050 retourne 0xA)
        rx_message_chs.data[6] = 0x00;                                         // low byte, Arr. DROITE
        rx_message_chs.data[7] = 0x0A;                                         // high byte, Arr. DROITE  // 254+10?
        break;
    }
  }

    // Modifier les trames si configuré en Gen2...
  if (haldexGeneration == 2) {  // Modifier les trames si configuré en Gen2.  Présentement copié de Gen4...
    switch (rx_message_chs.identifier) {
      case MOTOR1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[2] = 0x21;
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0xFE, false);
        break;
      case MOTOR3_ID:
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0x01, false);  // gen1 est 0xFE, gen4 est 0x01
        break;
      case MOTOR6_ID:
        break;
      case BRAKES1_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0x80, false);
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0x41, false);
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // gen1 est 0x00, gen4 est 0xFE
        rx_message_chs.data[3] = 0x0A;
        break;
      case BRAKES2_ID:
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0x7F, false);  // grand effet(!) 0x7F est le maximum
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // aucun effet.  Était 0x6E
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[1] = 0x0A;
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[3] = 0x0A;
        rx_message_chs.data[4] = 0x00;
        rx_message_chs.data[5] = 0x0A;
        rx_message_chs.data[6] = 0x00;
        rx_message_chs.data[7] = 0x0A;
        break;
    }
  }
  // Modifier les trames si configuré en Gen4...
  if (haldexGeneration == 4) {
    switch (rx_message_chs.identifier) {
      case mLW_1:
        rx_message_chs.data[0] = lws_2[mLW_1_counter][0];  // angle de braquage (bloc 011) low byte
        rx_message_chs.data[1] = lws_2[mLW_1_counter][1];  // aucun effet B high byte
        rx_message_chs.data[2] = lws_2[mLW_1_counter][2];  // aucun effet C
        rx_message_chs.data[3] = lws_2[mLW_1_counter][3];  // aucun effet D
        rx_message_chs.data[4] = lws_2[mLW_1_counter][4];  // taux de changement (bloc 010) était 0x00
        rx_message_chs.data[5] = lws_2[mLW_1_counter][5];  // aucun effet F
        rx_message_chs.data[6] = lws_2[mLW_1_counter][6];  // aucun effet F
        rx_message_chs.data[7] = lws_2[mLW_1_counter][7];  // aucun effet F
        mLW_1_counter++;
        if (mLW_1_counter > 15) {
          mLW_1_counter = 0;
        }
        break;
      case MOTOR1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);  // a un effet
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0x20, false);  // RPM low byte aucun effet était 0x20
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);  // RPM high byte.  Va désactiver la pompe de précharge si 0x00.  Règle raw = 8, couplage ouvert
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // MDNORM aucun effet
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // Pédale aucun effet
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0x16, false);  // "idle adaptation"?  Était patinage?
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0xFE, false);  // "Fahrerwunschmoment" torque requis?
        break;
      case MOTOR3_ID:
        //frame.data[2] = get_lock_target_adjusted_value(0xFE, false);
        //frame.data[7] = get_lock_target_adjusted_value(0x01, false);
        break;
      case BRAKES1_ID:
        rx_message_chs.data[0] = 0x20;                                         // ASR 0x04 règle l'octet 4.  0x08 retire le réglage.  Couplage ouvert/fermé
        rx_message_chs.data[1] = 0x40;                                         // peut utiliser pour désactiver (>130 dec).  Était 0x00; 0x41?  0x43?
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // Était 0xFE miasrl aucun effet
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // Était 0xFE miasrs aucun effet
      case BRAKES2_ID:
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0x7F, false);  // Grand effet(!) 0x7F est le maximum
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xB6, false);  // av. gauche low
        rx_message_chs.data[1] = 0x07;                                         // av. gauche high
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xCC, false);  // av. droite low
        rx_message_chs.data[3] = 0x07;                                         // av. droite high
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xD2, false);  // arr. gauche low
        rx_message_chs.data[5] = 0x07;                                         // arr. gauche high
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0xD2, false);  // arr. droite low
        rx_message_chs.data[7] = 0x07;                                         // arr. droite high
        break;

      case BRAKES4_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);  // affecte le torque estimé ET le mode du véhicule(!)
        rx_message_chs.data[1] = 0x00;                                         //
        rx_message_chs.data[2] = 0x00;                                         //
        rx_message_chs.data[3] = 0x64;                                         // 32605
        rx_message_chs.data[4] = 0x00;                                         //
        rx_message_chs.data[5] = 0x00;                                         //
        rx_message_chs.data[6] = BRAKES4_counter;                              // checksum
        BRAKES4_crc = 0;
        for (uint8_t i = 0; i < 7; i++) {
          BRAKES4_crc ^= rx_message_chs.data[i];
        }
        rx_message_chs.data[7] = BRAKES4_crc;

        BRAKES4_counter = BRAKES4_counter + 16;
        if (BRAKES4_counter > 0xF0) {
          BRAKES4_counter = 0x00;
        }
        break;
    }
  }
}
