#ifndef MOO_PIMPL_H
#define MOO_PIMPL_H

namespace moo {

namespace impl {

template<typename Pub, typename Priv>
class PointerP {
    Priv * const p;
public:
//     PointerP(Priv *p) : p(p) {}

    PointerP(Pub *q) : p(new Priv(q)) {}

    template<typename T1>
    PointerP(Pub *q, const T1 &a1) : p(new Priv(q, a1)) {}

    ~PointerP() { delete p; }

    operator Priv*() { return p; }
    operator const Priv* () const { return p; }
    Priv *operator->() { return p; }
    const Priv *operator->() const { return p; }
    Priv &operator*() { return *p; }
    const Priv &operator*() const { return *p; }
    Priv *data() { return p; }
    const Priv *data() const { return p; }

private:
    // prohibit calling it with 0
    PointerP(long nul);
};

template<typename Pub>
class PointerQ {
    Pub * const p;
public:
    PointerQ(Pub *p) : p(p) {}
    ~PointerQ() {}
    operator Pub*() { return p; }
    operator const Pub* () const { return p; }
    Pub *operator->() { return p; }
    const Pub *operator->() const { return p; }
    Pub &operator*() { return *p; }
    const Pub &operator*() const { return *p; }
    Pub *data() { return p; }
    const Pub *data() const { return p; }

private:
    // prohibit calling it with 0
    PointerQ(long nul);
};

}

#define M_DECLARE_IMPL(Class) \
protected: \
    moo::impl::PointerP<Class, Class##Private> impl; \
    Class##Private *m_dptr() { return impl.data(); } \
    const Class##Private *m_dptr() const { return impl.data(); } \
private: \
    friend class Class##Private;

#define M_DECLARE_PUBLIC(Class) \
protected: \
    moo::impl::PointerQ<Class> pub; \
    Class *m_qptr() { return pub.data(); } \
    const Class *m_qptr() const { return pub.data(); } \
private: \
    friend class Class; \
    friend class moo::impl::PointerP<Class, Class##Private>;

#define M_D(Class) Class##Private *d = m_dptr()
#define M_CD(Class) const Class##Private *d = m_dptr()
#define M_Q(Class) Class *q = m_qptr()
#define M_CQ(Class) const (Class) *q = m_qptr()

}

#endif // MOO_PIMPL_H
