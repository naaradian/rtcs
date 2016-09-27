//md100se_snmp.c

//extern  unsigned long TypeVer(void);

//extern RTCSMIB_NODE MIBNODE_TYPEVER;

//uint_32 MIB_set_TYPEVER             (pointer, uchar_ptr, uint_32);

     /*
RTCSMIB_NODE MIBNODE_TYPEVER = {
   1, //8,
   &MIBNODE_TYPEVER// &MIBNODE_CTRLNUMBER,
   NULL,
   &MIBNODE_MD100SE, //&MIBNODE_device,

 //  RTCSMIB_ACCESS_READ | RTCSMIB_ACCESS_WRITE,
   RTCSMIB_ACCESS_READ ,
   NULL,
   MIB_instance_zero, ASN1_TYPE_Counter,
   RTCSMIB_NODETYPE_INT_CONST, {0},
 //   MIB_set_DTVM_RATE_SET
    NULL
};
 */

uint_32 MIB_get_TYPEVER (pointer dummy)
{
 return TypeVer();
}
