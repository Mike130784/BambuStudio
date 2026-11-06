// Original offline research harness. No sockets, device commands, key files,
// Windows trust-store changes or Bambu credentials. This is NOT printer firmware.
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

template<class T, auto Free> using Owned = std::unique_ptr<T, decltype(Free)>;
using Key = Owned<EVP_PKEY, EVP_PKEY_free>;
using Cert = Owned<X509, X509_free>;
using Store = Owned<X509_STORE, X509_STORE_free>;
using Context = Owned<X509_STORE_CTX, X509_STORE_CTX_free>;

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
Key key() {
    Key k(EVP_RSA_gen(2048), EVP_PKEY_free);
    require(bool(k), "RSA key generation failed");
    return k;
}
Cert cert(EVP_PKEY* subject_key, const char* cn, long serial,
          X509* issuer, EVP_PKEY* issuer_key, bool ca) {
    Cert c(X509_new(), X509_free);
    require(bool(c), "Certificate allocation failed");
    require(X509_set_version(c.get(), 2) == 1, "Version failed");
    require(ASN1_INTEGER_set(X509_get_serialNumber(c.get()), serial) == 1, "Serial failed");
    require(X509_gmtime_adj(X509_getm_notBefore(c.get()), -60) != nullptr, "notBefore failed");
    require(X509_gmtime_adj(X509_getm_notAfter(c.get()), 3600) != nullptr, "notAfter failed");
    require(X509_set_pubkey(c.get(), subject_key) == 1, "Public key failed");
    auto* n = X509_get_subject_name(c.get());
    require(X509_NAME_add_entry_by_txt(n, "CN", MBSTRING_ASC,
        reinterpret_cast<const unsigned char*>(cn), -1, -1, 0) == 1, "Name failed");
    require(X509_set_issuer_name(c.get(), issuer ? X509_get_subject_name(issuer) : n) == 1,
            "Issuer failed");
    X509V3_CTX ctx{};
    X509V3_set_ctx(&ctx, issuer ? issuer : c.get(), c.get(), nullptr, nullptr, 0);
    auto add = [&](int nid, const char* value) {
        Owned<X509_EXTENSION, X509_EXTENSION_free> e(
            X509V3_EXT_conf_nid(nullptr, &ctx, nid, value), X509_EXTENSION_free);
        require(bool(e) && X509_add_ext(c.get(), e.get(), -1) == 1, "Extension failed");
    };
    add(NID_basic_constraints, ca ? "critical,CA:TRUE" : "critical,CA:FALSE");
    add(NID_key_usage, ca ? "critical,keyCertSign,cRLSign" : "critical,digitalSignature");
    require(X509_sign(c.get(), issuer_key ? issuer_key : subject_key, EVP_sha256()) > 0,
            "Certificate signing failed");
    return c;
}
int verify(X509* leaf, X509_STORE* store, STACK_OF(X509)* untrusted = nullptr) {
    Context ctx(X509_STORE_CTX_new(), X509_STORE_CTX_free);
    require(bool(ctx) && X509_STORE_CTX_init(ctx.get(), store, leaf, untrusted) == 1,
            "Verification initialization failed");
    const int result = X509_verify_cert(ctx.get());
    if (result == 1) return X509_V_OK;
    require(result == 0, "Unexpected verification engine error");
    return X509_STORE_CTX_get_error(ctx.get());
}
Store store() {
    Store s(X509_STORE_new(), X509_STORE_free);
    require(bool(s), "Trust store allocation failed");
    return s;
}
int main(int argc, char** argv) {
    try {
        require(argc == 2, "Provide the public vendor CA bundle path");
        auto root_key = key();
        auto root = cert(root_key.get(), "Custom Studio Research Root", 1, nullptr, nullptr, true);
        auto leaf_key = key();
        auto leaf = cert(leaf_key.get(), "Custom Studio Research Client", 2,
                         root.get(), root_key.get(), false);
        auto other_key = key();
        auto other = cert(other_key.get(), "Unrelated Research Root", 3, nullptr, nullptr, true);
        // Valid local mathematics and chain building do not establish remote trust.
        require(X509_verify(leaf.get(), root_key.get()) == 1, "Own issuer signature rejected");
        std::cout << "PASS own leaf has a valid issuer signature\n";
        auto own = store();
        require(X509_STORE_add_cert(own.get(), root.get()) == 1, "Own root failed");
        require(verify(leaf.get(), own.get()) == X509_V_OK, "Own chain rejected");
        std::cout << "PASS chain verifies when own root is explicitly trusted\n";
        auto unrelated = store();
        require(X509_STORE_add_cert(unrelated.get(), other.get()) == 1, "Other root failed");
        require(verify(leaf.get(), unrelated.get()) != X509_V_OK, "Unrelated root trusted leaf");
        std::cout << "PASS unrelated root rejects own chain\n";
        auto vendor = store();
        require(X509_STORE_load_locations(vendor.get(), argv[1], nullptr) == 1,
                "Public vendor bundle could not be parsed");
        require(verify(leaf.get(), vendor.get()) != X509_V_OK, "Vendor roots unexpectedly trust own leaf");
        std::cout << "PASS public vendor roots reject own leaf\n";
        auto* extra = sk_X509_new_null();
        require(extra != nullptr, "Untrusted stack failed");
        require(sk_X509_push(extra, root.get()) > 0, "Untrusted chain append failed");
        const int error = verify(leaf.get(), vendor.get(), extra);
        sk_X509_free(extra); // Certificate remains owned above.
        require(error != X509_V_OK, "Supplying own root established trust");
        std::cout << "PASS supplying own root as chain data still rejects: "
                  << X509_verify_cert_error_string(error) << "\n";
        std::cout << "5 PASS; offline X.509 model only, printer acceptance NOT RUN\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL " << e.what() << "\n";
        return 1;
    }
}
