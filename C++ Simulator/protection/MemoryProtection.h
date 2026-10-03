#ifndef MEMORYPROTECTION_H
#define MEMORYPROTECTION_H

#include <string>

enum class AccessType{READ,WRITE,EXECUTE};

class MemoryProtection
{
    bool readPermission;
    bool writePermission;
    bool executePermission;
    int violations;
public:
    MemoryProtection(bool r=true,bool w=false,bool x=false):readPermission(r),writePermission(w),executePermission(x),violations(0){}
    bool canAccess(AccessType type){bool ok=type==AccessType::READ?readPermission:type==AccessType::WRITE?writePermission:executePermission;if(!ok) ++violations;return ok;}
    void setPermissions(bool r,bool w,bool x){readPermission=r;writePermission=w;executePermission=x;}
    bool canRead() const{return readPermission;}bool canWrite() const{return writePermission;}bool canExecute() const{return executePermission;}int getViolations() const{return violations;}
    std::string getDescription() const{return std::string("R=")+(readPermission?"1":"0")+" W="+(writePermission?"1":"0")+" X="+(executePermission?"1":"0");}
};

#endif
