#include "val.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

Value Value::operator+(const Value &op) const {
  if (this->IsInt() && op.IsInt()) {
    Value result(this->GetInt() + op.GetInt());

    return result;
  } else if (this->IsReal() && op.IsInt()) {
    Value result(this->GetReal() + op.GetInt());

    return result;
  } else if (this->IsInt() && op.IsReal()) {
    Value result(this->GetInt() + op.GetReal());

    return result;
  } else if (this->IsReal() && op.IsReal()) {
    Value result(this->GetReal() + op.GetReal());

    return result;
  } else {
    Value result;
    return result;
  }
}

Value Value::operator-(const Value &op) const {
  if (this->IsInt() && op.IsInt()) {
    Value result(this->GetInt() - op.GetInt());

    return result;
  } else if (this->IsReal() && op.IsInt()) {
    Value result(this->GetReal() - op.GetInt());

    return result;
  } else if (this->IsInt() && op.IsReal()) {
    Value result(this->GetInt() - op.GetReal());

    return result;
  } else if (this->IsReal() && op.IsReal()) {
    Value result(this->GetReal() - op.GetReal());

    return result;
  } else {
    Value result;
    return result;
  }
}

Value Value::operator*(const Value &op) const {
  if (this->IsInt() && op.IsInt()) {
    Value result(this->GetInt() * op.GetInt());

    return result;
  } else if (this->IsReal() && op.IsInt()) {
    Value result(this->GetReal() * op.GetInt());

    return result;
  } else if (this->IsInt() && op.IsReal()) {
    Value result(this->GetInt() * op.GetReal());

    return result;
  } else if (this->IsReal() && op.IsReal()) {
    Value result(this->GetReal() * op.GetReal());

    return result;
  } else {
    Value result;
    return result;
  }
}

Value Value::operator/(const Value &op)
    const { // CHECK DIVISION BY ZERO CASE. and check integer division case
  if (this->IsInt() && op.IsInt()) {
    if (op.GetInt() == 0) {
      Value result;
      return result;
    }
    Value result(this->GetInt() / op.GetInt()); // conversion to double

    return result;
  } else if (this->IsReal() && op.IsInt()) {
    if (op.GetInt() == 0) {
      Value result;
      return result;
    }
    Value result(this->GetReal() / op.GetInt());

    return result;
  } else if (this->IsInt() && op.IsReal()) {
    if (op.GetReal() == 0) {
      Value result;
      return result;
    }
    Value result(this->GetInt() / op.GetReal());

    return result;
  } else if (this->IsReal() && op.IsReal()) {
    if (op.GetReal() == 0) {
      Value result;
      return result;
    }
    Value result(this->GetReal() / op.GetReal());

    return result;
  } else {
    Value result;
    return result;
  }
}

Value Value::operator==(const Value &op) const {
  bool lt = false;
  if (this->IsInt() && op.IsInt()) {
    lt = (this->GetInt() == op.GetInt());
  } else if (this->IsInt() && op.IsReal()) {
    lt = (this->GetInt() == op.GetReal());
  } else if (this->IsReal() && op.IsReal()) {
    lt = (this->GetReal() == op.GetReal());
  } else if (this->IsReal() && op.IsInt()) {
    lt = (this->GetReal() == op.GetInt());
  } else if (this->IsBool() && op.IsBool()) {
    lt = (this->GetBool() == op.GetBool());
  }
  /*else if (this->IsString() && op.IsString()) {
      lt = (this->GetString() > op.GetString());
  }*/
  else {
    Value result;
    return result;
  }
  Value result(lt);
  return result;
}

Value Value::operator>(const Value &op) const {
  bool lt = false;
  if (this->IsInt() && op.IsInt()) {
    lt = (this->GetInt() > op.GetInt());
  } else if (this->IsReal() && op.IsInt()) {
    lt = (this->GetReal() > op.GetInt());
  } else if (this->IsInt() && op.IsReal()) {
    lt = (this->GetInt() > op.GetReal());
  } else if (this->IsReal() && op.IsReal()) {
    lt = (this->GetReal() > op.GetReal());
  }
  /* else if (this->IsBool() && op.IsInt()) {
       lt = (this->GetBool() < op.GetInt());
   }
   else if (this->IsReal() && op.IsBool()) {
       lt = (this->GetReal() < op.GetBool());
   }
   else if (this->IsBool() && op.IsReal()) {
       lt = (this->GetBool() < op.GetReal());
   }
   else if (this->IsReal() && op.IsBool()) {
       lt = (this->GetReal() < op.GetBool());
   }*/
  else if (this->IsBool() && op.IsBool()) {
    lt = (this->GetBool() > op.GetBool());
  } else {
    Value result;
    return result;
  }
  Value result(lt);
  return result;
}

Value Value::operator<(const Value &op) const {
  bool lt = false;
  if (this->IsInt() && op.IsInt()) {
    lt = (this->GetInt() < op.GetInt());
  } else if (this->IsReal() && op.IsInt()) {
    lt = (this->GetReal() < op.GetInt());
  } else if (this->IsInt() && op.IsReal()) {
    lt = (this->GetInt() < op.GetReal());
  } else if (this->IsReal() && op.IsReal()) {
    lt = (this->GetReal() < op.GetReal());
  }
  /* else if (this->IsBool() && op.IsInt()) {
       lt = (this->GetBool() < op.GetInt());
   }
   else if (this->IsReal() && op.IsBool()) {
       lt = (this->GetReal() < op.GetBool());
   }
   else if (this->IsBool() && op.IsReal()) {
       lt = (this->GetBool() < op.GetReal());
   }
   else if (this->IsReal() && op.IsBool()) {
       lt = (this->GetReal() < op.GetBool());
   }*/
  else if (this->IsBool() && op.IsBool()) {
    lt = (this->GetBool() < op.GetBool());
  } else {
    Value result;
    return result;
  }
  Value result(lt);
  return result;
}

Value Value::operator&&(const Value &op) const {
  bool lt = false;
  if (this->IsBool() && op.IsBool()) {
    lt = (Btemp && op.GetBool());
  } else {
    Value result;
    return result;
  }
  Value result(lt);
  return result;
}

Value Value::operator||(
    const Value &op) const { // check for numbers cases, and Btemp might have to
                             // be this->GetBool()
  bool lt = false;
  if (this->IsBool() && op.IsBool()) {
    lt = (Btemp || op.GetBool());
  } else {
    Value result;
    return result;
  }
  Value result(lt);
  return result;
}

Value Value::operator!() const { // check again
  if (this->IsBool()) {
    Value result(!(this->GetBool()));
    return result;
  } else {
    Value result;
    return result;
  }
}