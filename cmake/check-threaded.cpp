//
// Created by kiva on 2018/6/3.
//

/**
 * Check whether compiler supports label address
 * @return 1 when supports, 0 when not
 */
int main() {
    void *p = &&label;
    goto *p;

    return 0;

    label:
    return 1;
}
