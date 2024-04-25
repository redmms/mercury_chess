
STD_HEADERS = bitset stdexcept iostream algorithm string fstream type_traits vector queue bit filesystem
for(hdr, STD_HEADERS) {
    STD_HEADER_UNITS += /headerUnit:angle $${hdr}=$${hdr}.ifc
}
