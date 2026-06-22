# TODO

Source of truth: `docs/todo.yaml`. Update YAML before starting or completing work.

## repo

- [x] Add docs/todo.yaml so the Modbus Zephyr module is tracked globally.
- [ ] Align repository docs, public API layout, and validation scripts with dephy_module_golden_sample.

## protocol

- [ ] Implement real Modbus RTU transport behind the public API.
- [ ] Implement real Modbus TCP transport behind the public API.

## validation

- [ ] Add parser, CRC, exception response, and timeout unit coverage.
- [ ] Add a Zephyr module metadata smoke script matching the golden sample.

## performance

- [ ] Review frame buffer allocation and reuse fixed buffers for bounded embedded memory behavior.
