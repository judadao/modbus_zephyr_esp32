# TODO

Source of truth: `docs/todo.yaml`. Update YAML before starting or completing work.

## repo

- [x] Add docs/todo.yaml so the Modbus Zephyr module is tracked globally.
- [x] Align repository docs, public API layout, and validation scripts with dephy_module_golden_sample.
- [x] Refactor repository until it passes the module golden sample structure audit.

## protocol

- [x] Implement real Modbus RTU transport behind the public API.
- [x] Implement real Modbus TCP transport behind the public API.

## validation

- [x] Add parser, CRC, exception response, and timeout unit coverage.
- [x] Add a Zephyr module metadata smoke script matching the golden sample.
- [x] Clean up cppcheck style findings in Modbus transfer and unit timeout coverage.

## performance

- [x] Review frame buffer allocation and reuse fixed buffers for bounded embedded memory behavior.
