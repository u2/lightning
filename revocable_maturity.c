#include "secp256k1_blake160.h"
#include "ckb_syscalls.h"
#include "ckb_consts.h"

// two of two multi signature
int main(int argc, char* argv[])
{
    int ret;
    uint64_t length = 0;
    uint8_t flag;
    uint64_t since;
    uint64_t size = 8;

    if (argc == 10) {
        // signed args:
        // arg[1] alice pubkey hash
        // arg[2] bob pubkey hash
        // arg[3] a or b

        // witnesses:
        // arg[4] alice pubkey
        // arg[5] alice signature
        // arg[6] alice signature size
        // arg[7] bob pubkey
        // arg[8] bob signature
        // arg[9] bob signature size

        length = *((uint64_t *) argv[6]);
        ret = verify_sighash_all(argv[1], argv[4], argv[5], length);
        if (ret != CKB_SUCCESS) {
            return ret;
        }

        length = *((uint64_t *) argv[9]);
        return verify_sighash_all(argv[2], argv[7], argv[8], length);
    }
    if (argc == 7) {
        // signed args:
        // arg[1] alice pubkey hash
        // arg[2] bob pubkey hash
        // arg[3] flag(a or b)

        // witnesses:
        // arg[4] pubkey
        // arg[5] signature
        // arg[6] signature size
        if (ckb_load_input_by_field(&since, &size, 0, 0, 1, CKB_INPUT_FIELD_SINCE) != CKB_SUCCESS) {
            return -98;
        }

        flag = *((uint8_t *) argv[3]);
        length = *((uint64_t *) argv[6]);

        char buf[32];
        memset(buf, 0, 32);
        snprintf(buf, 32, "since %d flag %d", since, flag);
        ckb_debug(buf);

        if (since < 10) {
            return -97;
        }

        if (flag == 0) {
            return verify_sighash_all(argv[1], argv[4], argv[5], length);
        } else {
            return verify_sighash_all(argv[2], argv[4], argv[5], length);
        }
    }
    return -99;
}