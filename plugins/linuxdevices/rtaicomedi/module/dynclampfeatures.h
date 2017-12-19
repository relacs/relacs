int get_features( void )
{
  int features = 0;
#ifdef ENABLE_COMPUTATION
  features |= 0x0001;
#endif
#ifdef ENABLE_MATHH
  features |= 0x0002;
#endif
#ifdef ENABLE_LOOKUPTABLES
  features |= 0x0004;
#endif
#ifdef ENABLE_TRIGGER
  features |= 0x0008;
#endif
#ifdef ENABLE_TTLPULSE
  features |= 0x0010;
#endif
#ifdef ENABLE_SYNCSEC
  features |= 0x0020;
#endif
#ifdef ENABLE_AITIME
  features |= 0x0040;
#endif
#ifdef ENABLE_AIACQUISITIONTIME
  features |= 0x0080;
#endif
#ifdef ENABLE_AOTIME
  features |= 0x0100;
#endif
#ifdef ENABLE_MODELTIME
  features |= 0x0200;
#endif
#ifdef ENABLE_WAITTIME
  features |= 0x0400;
#endif
  return  features;
}

void get_feature_str( char *featurestr )
{
#ifdef ENABLE_COMPUTATION
  strcat( featurestr, "COMPUTATION " );
#endif
#ifdef ENABLE_MATHH
  strcat( featurestr, "MATHH " );
#endif
#ifdef ENABLE_LOOKUPTABLES
  strcat( featurestr, "LOOKUPTABLES " );
#endif
#ifdef ENABLE_TRIGGER
  strcat( featurestr, "TRIGGER" );
#endif
#ifdef ENABLE_TTLPULSE
  strcat( featurestr, "TTLPULSE " );
#endif
#ifdef ENABLE_SYNCSEC
  strcat( featurestr, "SYNCSEC " );
#endif
#ifdef ENABLE_AITIME
  strcat( featurestr, "AITIME " );
#endif
#ifdef ENABLE_AIACQUISITIONTIME
  strcat( featurestr, "AIACQUISITIONTIME " );
#endif
#ifdef ENABLE_AOTIME
  strcat( featurestr, "AOTIME " );
#endif
#ifdef ENABLE_MODELTIME
  strcat( featurestr, "MODELTIME " );
#endif
#ifdef ENABLE_WAITTIME
  strcat( featurestr, "WAITTIME " );
#endif
}

